using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class WIPCamRedirect : MonoBehaviour
{
	[DllImport ("MyDLLAttempt")]
	private static extern void getOrientation (float[] data);
	[DllImport ("MyDLLAttempt")]
	private static extern void getOriSpeed3f (float[] data);
	[DllImport ("MyDLLAttempt")]
	private static extern void getSpeed (float[] data);
	[DllImport ("MyDLLAttempt")]
	private static extern int init ();
	[DllImport ("MyDLLAttempt")]
	private static extern void confirmMsg ();
	[DllImport ("MyDLLAttempt")]
	private static extern void getFPS (double[] data);
	[DllImport ("MyDLLAttempt")]
	private static extern void killBallCheck ();
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
	Quaternion refVirtual;
	Vector3 virtualDirection;
	Quaternion inter2;

    public void Start () {
		int a = init ();
		if (a == 555)
		{
			confirmMsg();
		}
		controller = GetComponent<CharacterController>();
		virtualDirection.Set (1, 0, 0);
		refVirtual = Quaternion.identity;
		inter2 = Quaternion.identity;
		// killBallCheck ();
    }

    public void Update () {

		if (Input.GetKeyDown (UnityEngine.KeyCode.A )) {
			killBallCheck();
		}



		float[] data = new float[4];
		getOrientation (data);

		Vector3 u;//, v, n;

		u.x = 0;u.y = 0;u.z = 0;
		//v.x = 0;v.y = 0;v.z = 0;
		//n.x = 0;n.y = 0;n.z = 0;
		u.Set (data[0],data[1],data[2]);

		getSpeed (data);
		float speed = 0;
		speed = data [0];

		var target = Quaternion.identity;//SetLook

		u.x *= -1;
		u = -u;

		if (Input.GetKeyDown (UnityEngine.KeyCode.B)) {
			setRefVirtual(u);
		}

		calcVirtual (u, speed);
		// virtualDirection agora tem a direçao que o usuario esta olhando
		target.SetLookRotation (virtualDirection);
		transform.rotation = target;

		controller.Move (virtualDirection * speed * Time.deltaTime);
	}
	
	void setRefVirtual(Vector3 a) {
		refVirtual.Set (a.x,a.y,a.z,0);
	}
	
	void calcVirtual(Vector3 a, float speed) {
		// Quaternion dirRef = Quaternion.identity;
		refVirtual.Set (a.x, a.y, a.z, 0);
		Quaternion dirAtt = Quaternion.identity;
		dirAtt.Set (a.x, a.y, a.z, 0);

		Quaternion inter1 = Quaternion.Slerp (refVirtual, dirAtt, speed);
		inter2 = Quaternion.Slerp (inter1, inter2, 0.5f);
		
		float ang = Quaternion.Angle (refVirtual, inter2);

		Quaternion RT = Quaternion.AngleAxis(ang, Vector3.up); // = Quaternion.identity;
		//Vector3 yAxis;
		//yAxis = Vector3 (0, 1, 0);

		virtualDirection = RT * virtualDirection;
	}
}
