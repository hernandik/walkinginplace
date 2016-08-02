using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class WipScript : MonoBehaviour
{
	[DllImport ("Wipdll")]
	private static extern void getOrientation (float[] data);
	[DllImport ("Wipdll")]
	private static extern void getOriSpeed3f (float[] data);
	[DllImport ("Wipdll")]
	private static extern void getSpeed (float[] data);
	[DllImport ("Wipdll")]
	private static extern int init ();
	[DllImport ("Wipdll")]
	private static extern void confirmMsg ();
	[DllImport ("Wipdll")]
	private static extern void getFPS (double[] data);
	[DllImport ("Wipdll")]
	private static extern void killBallCheck ();
	[DllImport ("Wipdll")]
	private static extern void enableCorrectBalPosition (int i);
	//private bool isHMDopen = false;
	//Quaternion test = new Quaternion();
	
	//public float adjAngle = -75;
	//Quaternion adj, adjY180, adjX90, adjY90; // Helper quaternions, for HMD rotation fix.
	
	//public getO()
	//public Quaternion getSensicsQuat() {
	//	// Gets quaternion from HMD.
	//	float[] data = new float[4];
	//	getDataSensicsHMD (data);
	
	//	return new Quaternion(data[2], -data[3], data[1], data[0]);
	//}
	Matrix4x4 M;
	CharacterController controller;
	
	public float wipSensitivity = 0.5f;
	private Quaternion inverseInitialRotation;
	private Vector3 playerDirection;
	private float playerSpeed;
	
	// Quaternion refVirtual;
	Vector3 refVirtual;
	Vector3 virtualDirection;
	Quaternion inter2;
	
	float angFilter;
	// giro virtual
	//Vector3  
	
	public void Start () {
		playerSpeed = 0;
		
		
		int a = init ();
		/*
		if (a == 555)
		{
			confirmMsg();
		}
		*/
		controller = GetComponent<CharacterController>();
		killBallCheck ();
		
		float[] data = new float[4];
		getOrientation (data);
		Vector3 u;
		getDirectionVector (data, out u);
		inverseInitialRotation = Quaternion.Inverse (Quaternion.LookRotation (u));
		
		virtualDirection.Set (-1, 0, 0);
		// refVirtual = Quaternion.identity;
		refVirtual.Set (1, 0, 0);
		inter2 = Quaternion.identity;
		angFilter = 0;
		
	}
	
	public void getDirectionVector(float[] data, out Vector3 u) {
		u = new Vector3 ();
		u.Set (data [0], data [1], data [2]);
		u.x *= -1;
		u = -u;
	}
	
	public void getSpeed(float[] data, out float speed) {
		speed = data [0];
	}
	
	public void Update () {
		
		if (Input.GetKeyDown (UnityEngine.KeyCode.A )) {
			killBallCheck();
		}
		
		float[] data = new float[4];
		Vector3 u = new Vector3();//, v, n;
		
		getOrientation (data);
		// u.x = 0;u.y = 0;u.z = 0;
		u.Set (data[0],data[1],data[2]);
		
		getSpeed (data);
		float speed = data[0];
		//speed = data [0];
		
		var target = Quaternion.identity;//SetLook
		
		u.x *= -1;
		u = -u;
		
		if (Input.GetKeyDown (KeyCode.F12)) {
			setRefVirtual(u);
			Debug.Log(" refVirtual" + refVirtual);
		}
		
		if (Input.GetKeyDown (KeyCode.F11)) {
			// setRefVirtual(u);
			enableCorrectBalPosition(0);
			// Debug.Log(" refVirtual" + refVirtual);
		}

		print (u);

		float ang = Vector3.Dot(refVirtual, u);

		ang = Mathf.Clamp (ang, -1.0f, 1.0f);
		ang = Mathf.Pow (Mathf.Abs (ang), 0.95f) * Mathf.Sign(ang);
		float angDegress = Mathf.Acos (ang);
		float speedWithBreak = Mathf.Pow (Mathf.Abs (ang), 1.5f) * speed;

		///Debug.Log ("ang " + ang);
		Vector3 cr = Vector3.Cross (refVirtual, u);
		if (cr.y < 0) {	// acos 0 e 1, e o -1?
			angDegress = -angDegress;
		}
		// float ang = Vector3.Angle(refVirtual, u); // nao funciona, nao sei por que ainda

		angDegress = Mathf.LerpAngle (0, angDegress, speed); // freia giro quando nao detecta passo
		
		// metodo vetorial
		Quaternion dirQ = Quaternion.AngleAxis (angDegress, Vector3.up);
		virtualDirection = dirQ * virtualDirection;
		/// angFilter = (float) Mathf.LerpAngle (angFilter, ang, 0.8f);
		
		// metodo trigonometrico, nao util
		// Vector3 VV = new Vector3(0, ang, 0);
		// transform.Rotate(VV);
		
		// virtualDirection agora tem a direçao que o usuario esta olhando
		target.SetLookRotation (virtualDirection);
		transform.rotation = target;
		
		controller.Move (virtualDirection * speedWithBreak * wipSensitivity * Time.deltaTime);
		// Vector3 SS = new Vector3 (speed * Time.deltaTime, 0, 0);
		// controller.SimpleMove (SS);	// speed * Time.deltaTime);
		// controller.Move(controller.transform.forward
		
	}
	
	void setRefVirtual(Vector3 a) {
		// refVirtual.Set (a.x,a.y,a.z);
		refVirtual = a;
		refVirtual.Normalize ();
	}
	
	/*void calcVirtual(Vector3 a, float speed) {
		// Quaternion dirRef = Quaternion.identity;
		Quaternion dirAtt = Quaternion.identity;
		dirAtt.Set (a.x, a.y, a.z, 0);
		
		Quaternion inter1 = Quaternion.Slerp (refVirtual, dirAtt, speed);
		inter2 = Quaternion.Slerp (inter1, inter2, 0.5f);
		
		float ang = Quaternion.Angle (refVirtual, inter2);
		
		Quaternion RT = Quaternion.AngleAxis(ang, Vector3.up); // = Quaternion.identity;
		//Vector3 yAxis;
		//yAxis = Vector3 (0, 1, 0);
		
		virtualDirection = RT * virtualDirection;
	}*/
	
}

