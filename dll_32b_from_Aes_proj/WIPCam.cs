using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class WIPCam : MonoBehaviour
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

    public void Start () {
		int a = init ();
		if (a == 555)
		{
			confirmMsg();
		}
    }

    public void Update () {
		//test = Quaternion.AngleAxis(adjAngle, Vector3.up);
		//if (this.gameObject.camera.enabled && isHMDopen) 
		//{
			// Apply the HMD's rotation to the Game Object.
			// transform.parent.localRotation = new Quaternion(qt.x, qt.y, qt.z, qt.w); 
    	//}
		float[] data = new float[4];
		getOrientation (data);
		Debug.Log ("Mensagem"+data[0]);


		Vector3 u, v, n;

		u.x = 0;u.y = 0;u.z = 0;
		v.x = 0;v.y = 0;v.z = 0;
		n.x = 0;n.y = 0;n.z = 0;

		u.Set (data[0],data[1],data[2]);
		v.Set (0, 1, 0);
		//n = Vector3.Cross (u, n);
		//M.SetRow (0, u);
		//M.SetRow (1, v);
		//M.SetRow (2, n);
		//float angle = Vector3.Angle (u, v);

		Vector3 axisR = new Vector3(0, 1, 0);

		var target = Quaternion.identity;//SetLook
		target.SetLookRotation (u);//Vector3(0,1,0));//.AngleAxis (angle, axisR);
		transform.rotation = target;// Set (0, angle, 0, 0);
		//transform.localRotation.ToAngleAxis (angle, axisR);
		//M.MultiplyVector (transform.rotation);
	}
}
