#ifndef _WIIBALBOARD
#define _WIIBALBOARD

//#include "filterLowPass_5-10hz.h"
#include <vector>
#include "super_Basic.h"
#include "derivada.h"
#include "wiiCap.h"
#include "matrix4x4f.h"
#include "vetorial.h"
#include "lowPassFilter.h"
#include "quaternion.h"

struct sensors_f
{
	union{
		struct{
			float	 TopL;
			float	 TopR;
			float BottomL;
			float BottomR;
			float Total; // sum of the 4 corner weights
		};
		struct{
			float p[5]; // acesso generico
		};
		struct{ // para giroscopio
			float	x;
			float	y;
			float	z;
			bool	but;
		};
	};
};
 
// dados enviados por esta estrutura via IPC
struct sendData
{
	char n;	// numero de balancas conectadas
	struct sensors_f sensor[5];
};

// controle de posicionamento das balanças
typedef struct 
{
	union{
		struct{
			vec3f p[4];
		};
		struct{
			vec3f TL,TR,BL,BR;
		};
	};
} bbStruct;

class wii //: public baseClass
{
	bool continueFlagVar;

	vec3f bbDim;
	vec3f bbStride;
	bbStruct *bb;	// armazena direção de cada sensor das balancas
	int nbb; // numero de balancas
	bool is2SaveData,is2LoadData, is2SaveProcData;
	FILE *debugDataFile;
	FILE *debugDataProcFile;
	
	short balPosIdx[4];

	float girAngFix;
	bool completeTurnEvent;
	int is2SaveProcDataCount;

	bool grafOn;
	bool stepOn;
	bool rotLockActive;
	

	vec3f gir;
	vec3f baseRedirectionVector;
#ifdef COMPLETEWII
	CAM debugCam;
	AMBIENTE *amb;
	lineObj *lineObjderX, *lineObjderZ, *lineObjmassX, *lineObjmassZ, *lineObjTotalWeight;
	combineBaseClass comb;
#endif

	typedef struct 
	{
		int pos;
		int top;
	} pulsePositionandTop;

	bool lastPulse;
	int lastPulseCount;
	int sampleCount;
	float lastPeakVal;
	pulsePositionandTop lastPT;
	std::vector<vec3f> derSaved;
	std::vector<pulsePositionandTop> validPulsePosition;

	int dertime;
	int isDirectionLimTh;
	vec3f debugEye;
	void procStepWithVarSpeed();

public:
	void waitContinueFlag();
	void continueFlag();

	void setBaseDirectionVector();
	void procBaseRedirection();
	// vec3f baseRedirected;
	Quat intermediateVecRedi[2];
	vec3f virtualDirection;

	void setDebugEye(vec3f a);
	vec3f *ypr;
	int arcoStatus;

	struct sensors_f *s1;
	int nBalConnectadas;

	struct sensors_f *sensorDataAnt;
	double clockIntAnt[2];
	double difTime;
	
	struct sensors_f *calibData;

#ifdef COMPLETEWII
	combineBaseClass combGrafDer;
	combineBaseClass combGrafMassa;
	
	// embelezamentos da visualização	
	arrow vetoresVisuais;
	lineObj *centroMassaVisual;//, *cMVG;
	lineObj *accVisual, *velVisual, *fatorAceleracaoVisual;
	lineObj *longTermPositionTracker;
	
	wii(AMBIENTE *a);
	lineObj *lineObjDeltaRotation;
#endif
	wii();
	derivada *vel, *acc;
	
	float angI;
	float limiarDer,limiarStep,speedFactor,fastSpeed;
	vec3f pos, posG;
	vec3f d, f;
	vec3f refVertex, refVertexA, refVertexB, refVertexF;
	vec3f refVertexI;
	float ang;
	//float E;
	vec3f angGir;
	float stepDecay;
	
	int stepCount;
	float topMag, topDerMag;

	float total;
	//float tempo;

	lowPass *filtro; // filtro geral de posição
	//lowPass *filtroG;
	lowPass *filtroDir;
	lowPass *limSpeed, *lowPassPesoNormal;
	bool stepPulseOn;

	vec3f longTermPosition;
	lowPass *longTermPositionFilter;
	
	vec3f outSpeed;	// auxiliar a E para gerar velocidade
	//lowPass *filtroGir;
	//lowPass *filtrogirAngFix;
	//lowPass *refFilterA, *refFilterB;
	

	void calib();
	//void proc();
	//void proc2BB();
	void proc4bb();
	void procGiro();
	void showTex();
	void show();
	void warningBands();
	void resetRefVec();
	
	// para execução offline, sem Balance boards
	void saveDataDebug();
	bool readDataDebug();

	void openDataDebug(char *a);
	void createDataDebugFile(char *s, bool noMsg=false);
	void stopDataDebug();
	void startSaveDataProc(char *s);

	void setInput1(struct sensors_f *a); // depreciado
	void setInput2(struct sensors_f *a); // depreciado
	void procDerivada();
	void procDerNew();
	void procStep();
	void config();
	void genWalkGraf(char *file);

	float stepBuffer;
	float stepBufferThreshold;
	float deltaRotation;
	float rotThreshold;
	
	float rotationThreshold;
	float magniScale;
	void procStepNew();
	
	int ciclos;
	bool enableAutomatedFixBalPosition;
	derivada *weightDer;
	int automatedFixStatus;
	void startAutomatedFixBalPosition();
	void procAutomatedFixBalPosition();

	bool fixBalPosition;
	int fixBalID;
	void enableCorrectBalPosition(int i);
	void procFixBalPosition();
	void balSwap(int a, int b);
	void getFixedSensorData(struct sensors_f *senData);

	char *msg;
};

#endif
