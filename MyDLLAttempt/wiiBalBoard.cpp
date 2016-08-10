//#include "main.h"
#include "stdafx.h"
#include "wiiBalBoard.h"
#include "constants.h"
#define NOECMENU // avoid use of MENU functions

void sensorInterpolate(struct sensors_f *a, struct sensors_f *b, float s, struct sensors_f *o);

void wii::calib()
{
	memcpy( &calibData[0], s1, sizeof(struct sensors_f)*5);
	resetRefVec();
}

void wii::resetRefVec()
{
	angGir.set(0,0,0);
	
	refVertexA.set(1,0,0);
	refVertexB.set(-1,0,0);
	refVertex = refVertexA-refVertexB; // correcao do eixo perpendicular
	refVertexI = refVertex;
	swap(refVertex.x, refVertex.z);
	refVertex.z = -refVertex.z;

	refVertexF.set(0,0,1);
}

wii::wii() {
	baseRedirectionVector.set(1,0,0);
	continueFlagVar = false;

	msg = new char[256];

	rotLockActive = false;
	fixBalPosition = false;
	fixBalID = 0;
	enableAutomatedFixBalPosition = false;
	virtualDirection.set(1,0,0);

	weightDer = new derivada();

	stepPulseOn = false;
	magniScale = 5;
	stepBufferThreshold = 0.3;
	stepBuffer = 0;
	rotationThreshold = 20;

	calibData = new struct sensors_f[5];
	memset(&calibData[0], 0, sizeof(struct sensors_f)*5);

	sensorDataAnt = new struct sensors_f[10];
	memset(&sensorDataAnt[0], 0, sizeof(struct sensors_f)*10);

	s1 = NULL;
	
	filtro = new lowPass(10); // 10
	filtroDir = new lowPass(10);
	filtroDir->aa = 0.1;	// alterado de 0.04
	
	lowPassPesoNormal = new lowPass(10);
	lowPassPesoNormal->aa = 0.9;

	longTermPositionFilter = new lowPass(120);
	longTermPositionFilter->aa = 0.02;
	
	limSpeed = new lowPass(2);
	limSpeed->aa = 0.2;

	resetRefVec();
	
	// Gera MAP de vetores das balanças
	// Distancia entre sensores - circulo base para gerar balança
	bbDim.set(1,0,0);
	bbStride.set(0.2f,0.2f, 0);
	bb = new bbStruct[4];
	bbStruct bbTmp;
	
	// gera posição sensores
	matrix4x4f k;
	k.identity();
	k.rotate(30.0f, vec3f(0,1,0));
	k.transformVector(&bbDim);
	bbTmp.p[0] = bbDim;
	k.rotate(60.0f*2.0f, vec3f(0,1,0));
	k.transformVector(&bbDim);
	bbTmp.p[1] = bbDim;
	
	k.identity();
	bbDim.set(1,0,0);
	k.rotate(-30.0f, vec3f(0,1,0));
	k.transformVector(&bbDim);
	bbTmp.p[2] = bbDim;
	k.rotate(-60.0f*2.0f, vec3f(0,1,0));
	k.transformVector(&bbDim);
	bbTmp.p[3] = bbDim;
	// encontra dimensoes maximas entre sensores
	vec3f v;
	findSizeV(&bbTmp.p[0], &v, 4);
	v = v * (0.5f);
	// gera posição absoluta para posicionar cada balança no espaço
	vec3f l[4];
	l[0] = (v * vec3f(1,1,1)) + vec3f(0.2f, 0, 0.2f);
	l[1] = (v * vec3f(1,1,-1)) + vec3f(0.2f, 0, -0.2f);
	l[2] = (v * vec3f(-1,1,1)) + vec3f(-0.2f, 0, 0.2f);
	l[3] = (v * vec3f(-1,1,-1)) + vec3f(-0.2f, 0, -0.2f);
	
#ifdef LOGS
	fprintf(logs, "POSIÇÂO SENSORES BALANCA\n");
#endif
	
	for(int i=0; i<4; i++) balPosIdx[i]=i;
	// aplica posição absoluta a cada balanca
	for(int i = 0; i<4; i++)
	{
		memcpy(bb[i].p, &bbTmp.p[0], sizeof(vec3f)*4);
		translate(&bb[i].p[0], l[i], 4);
		// salva dados de posição dos sensores
#ifdef LOGS
		fprintf(logs, "balance %d\n", i);
		for(int j=0; j<4; j++)
			fprintf(logs, "s %d X %f Y %f Z %f\n", j, bb[i].p[j].x, bb[i].p[j].y, bb[i].p[j].z);
#endif
	}

	fastSpeed = 1000;
	speedFactor = 0.0;
	limiarDer = 0.15;
	limiarStep = 0.15;

	stepCount = 0;	// quantos steps ocorreram
	stepDecay = 0.8;	// decaimento da velocidade
	stepOn=false;	// quando ocorre um passo é disparado
	total = 0.0;
	
	is2SaveData = false;
	is2LoadData = false;
	is2SaveProcData = false;
	topDerMag = 0;
	topMag = 0;
	ypr = NULL;
	is2SaveProcDataCount = 0;
	vel = new derivada;
	acc = new derivada;

	// configura cores graficos
	f.set(1,0,0);
#ifndef NOECMENU
	eC.registerEvent(&combGrafDer); // grafico de linhas com a velocidade e a aceleração
	eC.registerEvent(&combGrafMassa); // grafico com o centro de massa e posição das balancas
#endif
#ifndef NOECMENU
	eC.registerEvent(&comb);
#endif
	dertime = 0;
	isDirectionLimTh = 0;
	deltaRotation = 0;
	rotThreshold = 20;	// estipulado aleatoriamente
}

void wii::setInput1(struct sensors_f *a)
{
	s1 = a;
}

void wii::config()
{

}

// dados do giroscopio para ser exibido
void wii::procGiro()
{
	matrix4x4f t;
	f.set(1,0,0);
	t.translate(vec3f(0,0,-5));
	t.rotate(angGir.y, vec3f(0,0,1));
	t.transformVector(&f);
}

#ifndef _SHOWCASE

void wii::setDebugEye(vec3f a)
{
	debugEye = a;
}

void wii::proc4bb()
{
	if(	is2SaveData )
		saveDataDebug();

	if(is2LoadData)
		readDataDebug();

	if(fixBalPosition) {
		// se estiver habilitado o modo de ajuste de balanca, entao chama a funcao de verificação
		procFixBalPosition();
	}

	total = s1[0].Total + s1[1].Total + s1[2].Total +s1[3].Total;
	vec3f TOTAL;
	TOTAL.x = total;
	lowPassPesoNormal->updateIIR(&TOTAL);
	total = TOTAL.x;

	if (total < 10.0)
		total = 10.0f;
	
	procAutomatedFixBalPosition();	// teste interno de execucao ou nao

	pos.set(0,0,0);
	
	float T = 1.0f/total;
	
	// Calcula centro de massa para n balancas(4)
	//	baseado nas informações previamente calculadas
	//	e retorna em pos a posição
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			// balPosIdx[i] nova versao com index - 30-08-2014
			pos = pos + ((s1[balPosIdx[i]].p[j] * bb[i].p[j]) * T);
		}
	}
	
	// 2 calculos para centro de massa, para analise
	filtro->update(&pos); // filtragem normal bandpass 33hz aprox
	//filtroG->updateIIR(&posG);	// filtragem por Infinite Impulse Response
	longTermPosition = pos;

	vel->add(&pos, clock); // versao com filtro exponencial
	acc->add(&vel->der, clock);
	//longTermPositionFilter->updateIIR(&posN);
	longTermPositionFilter->updateIIR(&longTermPosition);
	// processamento efetivo da posição
	//procDerivada(&vel->der);
	d = vel->der; // salva derivada localmente
	//procStep();
	procDerNew();
	//procStepNew();
	procStepWithVarSpeed();
}

void wii::procDerivada()
{
	angI=0;
	if(d.len() > limiarDer)
	{
		stepOn= true;
		angI = dotProduct(d, refVertex)/ ( d.len()*refVertex.len());
		if(angI <= -1.0) // limita range
		{
			angI = M_PI; // está a 180 graus
		}else{
			angI = acosf(angI);
		}

		if((angI > M_PI_2)) // passou limiar 90 graus
		{
			refVertex = d * -1;
			//printf("ang#%5.5f\n", angI/piover180);
		}else{
			refVertex = d;
			//printf("ang %5.5f\n", angI/piover180);
		}
		
		refVertex.norm();
		
		
	}else{
		stepOn=false;
		//E *= stepDecay;
	}
}

void wii::startAutomatedFixBalPosition()
{
	if(!enableAutomatedFixBalPosition) {
		enableAutomatedFixBalPosition = true;
		automatedFixStatus = 0;
	}
}

void wii::continueFlag() {
	continueFlagVar = false;
	// MessageBox(NULL, "Recebido continueFlag", "Aviso", MB_OK);
}

void wii::waitContinueFlag() {
	int i=0;
	
	// nao precisa mais
	// while(continueFlagVar == true){};
}

void wii::procAutomatedFixBalPosition()
{
	if(!enableAutomatedFixBalPosition)
		return; // nao habilitado
	// ask for user get on top of first balance, and second, and so on
	// keep track of weight of the user

	vec3f intTot;
	// continueFlagVar = false;

	switch(automatedFixStatus) {
	case 0:
		// pede para subir
		sprintf(msg, "ATENÇÃO: Suba na primeira balanca!\n");
		writeLogger(msg);
		continueFlagVar = true;
		waitContinueFlag();
		// MessageBox(NULL, "Suba na primeira balanca", "Aviso", MB_OK);
		automatedFixStatus = 1;
		ciclos=0;
		break;
	case 1:
		// fica monitorando o peso até que fique estavel por 1 segundo.....
		//  e acima de 20KG (instabilidade  < 2KG)!
		intTot.x = total;
		weightDer->add(&intTot,0.0);
		// monitor weight looking for changes... if happen
		if(total < 20) {	// nada ainda
			break;
		}else {
			ciclos++;
			if(ciclos < 300) {	// aguarda 3 segundos antes de qualquer coisa
				break;
			}
			if(weightDer->der.x < 1) {
				// MessageBox(NULL, "Suba na primeira balanca", "Aviso", MB_OK);
				// verifica se esta balanca é a balança certa
				enableCorrectBalPosition(0); // começa pela 1 depois vai pra 0 e entao para a 2
				automatedFixStatus = 2;
			}
		}
		break;
	case 2:
		// aguarda pelo retorno da função de troca automatizada de posição
		if(fixBalPosition) {
			break;
		}
		// retornou, balanças trocadas de posição
		// move para novo status
		automatedFixStatus = 3;
		break;
	case 3:
		// checa se tem mais de 2 balanças conectadas, caso contrario termina por aqui o processo
		if(nBalConnectadas < 3) {
			automatedFixStatus = 0;
			enableAutomatedFixBalPosition = false;
			sprintf(msg, "Terminada Calibração!\n");
			writeLogger(msg);
			continueFlagVar = true;
			waitContinueFlag();
			break;
		}else {
			// continua, aguarda usuario mudar de balança... espera-se que ele esteja apenas sobre 1 delas
			// check if signal derivative is stable or changing--- easier, is already done
			sprintf(msg, "ATENÇÃO: Avance uma balança a frente!\n");
			writeLogger(msg);
			continueFlagVar = true;
			waitContinueFlag();
			automatedFixStatus = 4;
			// senao keep waiting...
		}
		break;
	case 4:
		if(vel->der.len() > 0.7) {	// pico de mudança de plataforma
			automatedFixStatus = 5;
			ciclos = 0;
			// MessageBox(NULL, "Detectado Movimento, aguardando estabilizar!", "Aviso", MB_OK);
		}
		break;
	case 5:
		// usuario está se movendo de balança, quando a derivada de velocidade estabilizar
		// testa nova balança, a 0 neste caso
		if(total < 20) {	// nada ainda
			break;
		}
		if(vel->der.len() < 0.2) {	// 0.3? super restritivo... talvez nao funcione
			ciclos++;
			if(ciclos > 300) {
				sprintf(msg, "Estabilizado o efetuando correção!\n");
				writeLogger(msg);
				continueFlagVar = true;
				waitContinueFlag();
				// MessageBox(NULL, "Estabilizadoooo!", "Aviso", MB_OK);
				enableCorrectBalPosition(1);
				automatedFixStatus = 6;	// aguarda terminar troca
			}
		}
		break;
		// senao fica esperado o usuario parar quieto
	case 6:
		if(!fixBalPosition) {
			// usuario parou quieto e a balança teve sua posição corrigida
			if(nBalConnectadas < 3) {
				automatedFixStatus = 0;
				enableAutomatedFixBalPosition = false;
				sprintf(msg, "Terminado!\n");
				writeLogger(msg);
				continueFlagVar = true;
				waitContinueFlag();
				// MessageBox(NULL, "Terminado!", "Aviso", MB_OK);
				break;
			}else if(nBalConnectadas >= 3) {
				sprintf(msg, "ATENÇÃO: Mova-se para a balança a sua Direita (caso nao exista, voce começou a calibração de maneira incorreta, verifique o manual)!\n");
				writeLogger(msg);
				continueFlagVar = true;
				waitContinueFlag();
				// MessageBox(NULL, "Mova-se para a balança da direita!", "Aviso", MB_OK);
				automatedFixStatus = 7;
			}
		} // senao aguarda
		break;
	case 7:
		if(vel->der.len() > 0.7) {	// pico de mudança de plataforma
			automatedFixStatus = 8;
			ciclos = 0;
			// MessageBox(NULL, "Detectado Movimento, aguardando estabilizar!", "Aviso", MB_OK);
		}
		break;
	case 8:
		// usuario está se movendo de balança, quando a derivada de velocidade estabilizar
		// testa nova balança, a 0 neste caso
		if(total < 20) {	// nada ainda
			break;
		}
		if(vel->der.len() < 0.2) {	// 0.3? super restritivo... talvez nao funcione
			ciclos++;
			if(ciclos > 300) {
				sprintf(msg, "Estabilizado! Fazendo ajustes finais.\n");
				writeLogger(msg);
				continueFlagVar = true;
				waitContinueFlag();
				// MessageBox(NULL, "Estabilizadoooo!", "Aviso", MB_OK);
				enableCorrectBalPosition(3);
				automatedFixStatus = 9;	// aguarda terminar troca
			}
		}
		break;
		// senao fica esperado o usuario parar quieto
	case 9:
		// fixBalPosition é o status de outra função que ficar rodando em segundo plano,
		// entao temos que esperar ela retornar...
		if(!fixBalPosition) {	// usuario parou quieto e a balança teve sua posição corrigida
			if(nBalConnectadas >= 3) {
				automatedFixStatus = 0;
				enableAutomatedFixBalPosition = false;
				sprintf(msg, "Terminado!\n");
				writeLogger(msg);
				continueFlagVar = true;
				waitContinueFlag();
				// MessageBox(NULL, "Terminado!", "Aviso", MB_OK);
				break;
			}
		} // senao aguarda
		break;
	}
}

void wii::setBaseDirectionVector() {
	baseRedirectionVector = refVertex;
}

void wii::procBaseRedirection() {
	// calcula 'baseRedirected'
	// compara baseRedirectionVector e refVertex
	// a diferença entre eles é utilizada para fazer baseRedirected girar
	// intermediateVecRedi = mix(intermediateVecRedi,
	Quat refVertQ, baseRedirectionVectorQ;
	refVertQ = refVertex;
	baseRedirectionVectorQ = baseRedirectionVector;

	intermediateVecRedi[0] = QuatSlerp(baseRedirectionVectorQ, refVertQ, clamp(outSpeed.x,0.0f, 1.0f));
	intermediateVecRedi[1] = QuatSlerp(intermediateVecRedi[0],intermediateVecRedi[1], 0.5); // da uma amenizada
	// verifica a diferenca entre angulos
	vec3f V1; V1 = intermediateVecRedi[1];
	float angDiff = dotProduct(baseRedirectionVector, V1);

	// pow com expoente menor q 1 torna curva
	angDiff = pow((float)angDiff, 0.05f); // minimiza efeito do giro
	
	matrix4x4f M;
	M.rotate(acos(angDiff), vec3f(0,1,0));
	M.transformVector(&virtualDirection);
}

void wii::procFixBalPosition()
{
	// check if the weight grow bigger in one balance from the ground
	// is so, check if this balance is really how it should be
	// s1[0].Total + s1[1].Total + s1[2].Total +s1[3].Total;
	int balSelected = 999;
	for(int i = 0; i < 4; i++) {
		if(s1[balPosIdx[i]].Total > 20)
		{
			balSelected = i;
			break;
		}
	}

	if(balSelected != 999) {
		if(balSelected == fixBalID) {
			// nao precisa corrigir o numero da balanca, já está certo
			wsprintf(msg, "Balança na posição CORRETA!");
			writeLogger(msg);
			//MessageBox(NULL, "Troca NÃO necessaria", "Aviso", MB_OK);
			fixBalPosition = false; // finalizado
		}else{
			// faz swap da balanca em questão, pois está errada
			wsprintf(msg, "Balança na posição INCORRETA, corrigindo automaticamente!");
			writeLogger(msg);
			balSwap(balSelected, fixBalID);
			fixBalPosition = false; // finalizado
		}
	}
	return;
}

void wii::getFixedSensorData(struct sensors_f *senData) {
	for(int i=0; i < 4; i++) {
		memcpy( &senData[i], &s1[balPosIdx[i]], sizeof(struct sensors_f));
	}
}

void wii::balSwap(int b1, int b2)
{
	// apenas swap dos indexes
	int t = balPosIdx[b1];
	balPosIdx[b1] = balPosIdx[b2];
	balPosIdx[b2] = t;
	return;
}

// recebe o numero da balanca e faz a toca ao detectar peso suficiente sobre a balanca certa
void wii::enableCorrectBalPosition(int i)
{
	// configuração das balanças
	//l[0] = (v * vec3f(1,1,1)) + vec3f(0.2f, 0, 0.2f);
	//l[1] = (v * vec3f(1,1,-1)) + vec3f(0.2f, 0, -0.2f);
	//l[2] = (v * vec3f(-1,1,1)) + vec3f(-0.2f, 0, 0.2f);
	//l[3] = (v * vec3f(-1,1,-1)) + vec3f(-0.2f, 0, -0.2f);
	/*
				[2][0]
				[3][1]
	*/

	fixBalPosition = true;
	fixBalID = i;
}

// Gera vetor outSpeed contendo
// velocidade do usuario
void wii::procStepWithVarSpeed()
{
	float magni = d.len();
	
#ifdef LOGS
	gotoxys(0,3); printf("magni %f\n", magni);
#endif
	// step detection
	if(magni > limiarStep)
	{
		stepOn = true;
		stepBuffer = 1;
	}else
	{
		stepOn = false;
	}

	if(stepOn && !stepPulseOn)
	{
		stepPulseOn = true;
	}else if (!stepOn)
	{
		stepPulseOn = false;
	}
	if(stepPulseOn) {
		// para controle de velocidade
		speedFactor += 0.005;
		if(speedFactor > 1.0)
			speedFactor = 1.0;
	}

	vec3f a;
	a.set(magni,0,0);
	
	// Nao sei se e realmente necessario. 
	// talvez andar com qualquer movimento seja melho
	if(magni > limiarStep && stepBuffer > stepBufferThreshold)	
	{
		// caminha neste caso
		// magni é tratado apenas quando é maior que 0.15
		// entao ele deve chegar entre 0.15 ele é usado como fator no calculo da velocidade do passo
		// talvez uma funcao de pow para limitar o tamanho e o range de magni
		outSpeed.x = deltaS * magni * magniScale; // amplifica um pouco a magnitude
	}else {
		// decay step (fixo por momento)
		outSpeed.x *= stepDecay;
	}

	limSpeed->updateIIR(&outSpeed);
	stepBuffer = stepBuffer * 0.977237;	// configurado para 100hz
	speedFactor *= 0.98f;

#ifdef LOGS
	gotoxys(10,10); printf("deltaRotation: %f", deltaRotation/piover180);
#endif

	if((deltaRotation/piover180) > rotationThreshold)	// cancela passo caso delta rotation seja grande
		stepBuffer *= 0.1;
}

void wii::procStepNew()
{
	float magni = d.len();
#ifdef LOGS
	gotoxys(0,3); printf("magni %f\n", magni);
#endif
	// step detection
	if(magni > limiarStep)
	{
		stepOn = true;
		stepBuffer = 1;
		// para controle de velocidade
		speedFactor += 0.005;
		if(speedFactor > 1.0)
			speedFactor = 1.0;
		
	}else
	{
		stepOn = false;
	}

	if(stepBuffer > stepBufferThreshold)
	{
		// caminha neste caso
		outSpeed.x = deltaS * stepBuffer;//*speedFactor;	// alterado!!!!!

	}else
	{
		// decay step (fixo por momento)
		outSpeed.x *= 0.75;
	}

	stepBuffer = stepBuffer * 0.977237;	// configurado para 100hz
	speedFactor *= 0.98f;

#ifdef LOGS
	gotoxys(10,10); printf("deltaRotation: %f", deltaRotation/piover180);
#endif	
	if((deltaRotation/piover180) > rotationThreshold)	// cancela passo caso delta rotation seja grande
		stepBuffer *= 0.1;
}

void wii::procStep()
{
	// Processa centro massa a procura de side walk
	float magni = d.len();
#ifdef LOGS
	gotoxys(0,3); printf("magni %f\n", magni);
#endif	
	//fprintf(logs, "magni %f\n", magni);

	vec3f g;
	g = pos - longTermPosition;

	float gMag=g.len();
	
	// step detection
	if(magni > limiarDer)
	{
		stepOn = true;
		// registra topo do sinal
		if(gMag > topMag)
			topMag = gMag;
		if(magni > topDerMag)
			topDerMag = magni;

	}else
	{
		if(stepOn)
		{
			stepCount++;
			stepOn = false;
		}
	}

	outSpeed.x *= stepDecay;
	outSpeed.y *= stepDecay;
#ifdef LOGS
	gotoxys(0,12); printf("Step %d top mag %f ", stepCount, topMag);
#endif
	if(stepCount > 0)
	{
		if(topMag >= 0.8)	// side walk ocorreu
		{
			vec3f cr;
			vec3f D = d;

			D.norm();
			cr = crossProduct(d, refVertex);

			if(cr.y > 0)
			{
				outSpeed.y = deltaS;
				gotoxys(0,13);
				printf(" >");
			}
			else
			{
				outSpeed.y = -deltaS;
#ifdef LOGS
				gotoxys(0,13);
				printf(" <");
#endif
			}

			stepCount --; // para deixar negativo o contador de passos, assim força o cancelamento de 1 passo
#ifdef LOGS
			printf("a");
#endif
		}
		else if(topDerMag > limiarDer) //(topMag > limiarDer && topMag < 0.8)	// step normal ocorreu
		{
			outSpeed.x = deltaS;
			gotoxys(0,13);
#ifdef LOGS
			printf("b      ");
#endif
		}else
		{
			gotoxys(0,13); printf("                     ");
		}
		topMag = 0.0f;
		topDerMag = 0.0f;
		stepCount --;
	}

	outSpeed.y = 0; // anula side walk
}

void wii::procDerNew() {
	angI=0;
	float magni = d.len();

	// processa direção passo
	if(magni > limiarDer) {
		// processa sinal apenas nos primeiros ms que ultrapassa o limiar.
		if(isDirectionLimTh < 6) {
			
			//if(isDirectionLimTh < 5)
				isDirectionLimTh++;

			angI = dotProduct(d, refVertexF)/ ( d.len()*refVertexF.len());
			if(angI <= -1.0) {// limita range
				// está a 180 graus
				angI = M_PI;
			}else {
				angI = acosf(angI);
			}

			// gera vetores de controle
			if((angI > M_PI_2)) { // passou limiar 90 graus
				refVertexA = d;	// * -1;
				refVertexF = -d;	// * -1;
				//printf("ang#%5.5f\n", angI/piover180);
			} else {
				refVertexB = d;
				refVertexF = d;
				//printf("ang %5.5f\n", angI/piover180);
			}
			
			//refVertexA.norm();
			//refVertexB.norm();

			refVertexI = refVertexB-refVertexA;//*vec3f(-1,1,1); // correcao do eixo perpendicular
			swap(refVertexI.x, refVertexI.z);
			//refVertex.z = -refVertex.z;
			refVertexI.norm();	
		} else if(isDirectionLimTh >= 2 || rotLockActive) {
			rotLockActive = true;
		}
		
	}else { // if((isDirectionLimTh >= 2) || rotLockActive)
		isDirectionLimTh=0;
		deltaRotation = 0;
		rotLockActive = false;	// destava para proximo passo
	}

	refVertex = refVertexI; // update
	filtroDir->updateIIR(&refVertex); // usado super filtro IIR
	//limSpeed->updateIIR(&outSpeed);
	deltaRotation = acosf(dotProduct(refVertexI, refVertex));
}

#else

void wii::procDerivada(vec3f *a){}
void wii::procDerNew(vec3f *a){}
void wii::proc4bb(){}
void wii::proc2BB(){}

#endif

// para guardar posições de vertices e cores temporariamente na
// função genOneWalkGraf
typedef struct cc{
	vec3f v,c;
} CC;


// Rotinas para uso com debug
void wii::saveDataDebug()
{
	fwrite(&clock, 1, sizeof(double), debugDataFile);
	fwrite(s1, 1, sizeof(struct sensors_f)*5, debugDataFile);
	//printf("w: %d sizeof %d\n", a, sizeof(*s1));
}

bool wii::readDataDebug()
{
	if((clockIntAnt[1] + difTime) >= clock)
	{
		// usar dados antigos, busca ainda dentro da janela
		// não atualiza nada
		//fprintf(logs, "-");
		printf("-");
	}else
	{
		// dados fora da janela, buscar dados novos
		// até encontrar intervalo valido
		//fprintf(logs, "l");
		do{
			//fprintf(logs, "L");
			gotoxys(0,0);printf("t %f clock %f diff %f\n", clockIntAnt[0], clock, clockIntAnt[1]-clockIntAnt[0]);
			// salva dado anterior
			clockIntAnt[0] = clockIntAnt[1];
			memcpy(&sensorDataAnt[0], &sensorDataAnt[5], sizeof(struct sensors_f)*5);

			// le novos dados
			fread(&clockIntAnt[1], 1, sizeof(double), debugDataFile);
			fread(&sensorDataAnt[5], 1, sizeof(struct sensors_f) * 5, debugDataFile);

			// rebobina
			if(feof(debugDataFile))
			{
				rewind(debugDataFile);

				fread(&clockIntAnt[0], 1, sizeof(double), debugDataFile);
				fread(&sensorDataAnt[0], 1, sizeof(struct sensors_f) * 5, debugDataFile);
				fread(&clockIntAnt[1], 1, sizeof(double), debugDataFile);
				fread(&sensorDataAnt[1], 1, sizeof(struct sensors_f) * 5, debugDataFile);

				difTime = clock - clockIntAnt[0];
				
				if(grafOn == true)
				{
					return true;
				}
				// stop play the data... for good
				stopDataDebug();
				MessageBoxW(S->hWnd, L"Terminou ler FITA :)", L"AVISO", MB_OK);
			}
			//printf("%ld\n", clock);
		}while((clockIntAnt[1] + difTime) < clock);
	}

	double r = clock - (clockIntAnt[0]+difTime);
	float s = r / (double) (clockIntAnt[1]-clockIntAnt[0]);
	sensorInterpolate(&sensorDataAnt[0], &sensorDataAnt[5], s, s1);
	
	return false; // modo normal
}

void sensorInterpolate(struct sensors_f *a, struct sensors_f *b, float s, struct sensors_f *o)
{
	for(int i = 0; i < 5; i++)
	{
		o[i].BottomL = mix(a[i].BottomL, b[i].BottomL, s);
		o[i].BottomR = mix(a[i].BottomR, b[i].BottomR, s);
		o[i].TopL = mix(a[i].TopL, b[i].TopL, s);
		o[i].TopR = mix(a[i].TopR, b[i].TopR, s);
		o[i].Total = mix(a[i].Total, b[i].Total, s);
	}
}

void wii::openDataDebug(char *a)
{
	if(!a)
	{
		return;
	}

	debugDataFile = fopen(a, "rb");

	if(!debugDataFile)
	{
		MessageBoxW(S->hWnd, L"Erro abrindo arquivo de debug", L"Erro", MB_OK);
		return;
	}

	is2SaveData = false;
	is2LoadData = true;

	fread(&clockIntAnt[0], 1, sizeof(double), debugDataFile);
	fread(&sensorDataAnt[0], 1, sizeof(struct sensors_f) * 5, debugDataFile);
	fread(&clockIntAnt[1], 1, sizeof(double), debugDataFile);
	fread(&sensorDataAnt[1], 1, sizeof(struct sensors_f) * 5, debugDataFile);
	
	//pClock(); // assegura clock correto
	difTime = clock - clockIntAnt[0]; // clock atual menos o clock do arquivo
#ifdef LOGS
	fprintf(logs, "clock0 %f\n",clockIntAnt[0]);
	fprintf(logs, "diff %f\n", difTime);
#endif
}

void wii::createDataDebugFile(char *s, bool noMsg)
{
	if(!noMsg)
	{
		if(MessageBoxW(S->hWnd, L"Tem certeza que deseja sobrescrever o arquivo?", L"Aviso", MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
		{
			return;
		}
	}

	debugDataFile = fopen(s, "wb");//fopen("signalData.dat", "wb");
	
	if(!debugDataFile)
	{
		MessageBoxW(S->hWnd, L"Erro abrindo arquivo de debug", L"Erro", MB_OK);
		return;
	}

	is2LoadData = false;
	is2SaveData = true;
}

void wii::stopDataDebug()
{
	if(is2LoadData || is2SaveData)
		fclose(debugDataFile);

	is2LoadData = false;
	is2SaveData = false;

	if(is2SaveProcData)
		fclose(debugDataProcFile);
	is2SaveProcData = false;
}

void wii::startSaveDataProc(char *s)
{
	debugDataProcFile = fopen(s, "wt");//fopen("signalData.dat", "wb");
	if(!debugDataProcFile)
	{
		MessageBoxW(S->hWnd, L"Erro abrindo arquivo de debug", L"Erro", MB_OK);
		return;
	}

	lastPulse = false;
	lastPulseCount = 0;
	sampleCount = 0;
	derSaved.clear();	// limpeza
	validPulsePosition.clear();

	is2SaveProcData = true;
}
