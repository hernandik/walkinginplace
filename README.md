# walkinginplace
Technique for detecting walking in place with multiple Wii balance Board

This repositories contains a VC2010 project with the technique that I develop to detect orientation and speed, of a person, on top of a set of balance boards. Up to four balances can be connectec right now.
This technique is in the walking in place field of research of humam machine interaction.

This repo may contains some proprietary code, and contains code from WiiYourself 1.15.

This project generates a DLL to be utilized bu your application.
There is also a example of usage of the DLL in Unity 3D

To start using, a set of procedures should be done in the begining of usage, to connect and calibrate the balances.
The DLL emmit status information through an internal text buffer, exposed to load application.

I am sharing this code, to make my project free to anyone interested in use it.
This code contains WiiiYourSelf code.
At this moment, I believe that it need some companion DLL libraries not included.
* To compile this project you need WinDDK 7.1

Further updates will be provided to this project, as it is very messy now.

# USAGE:

Load the DLL inside your unity project based on the WIPCam.cs C# script
To test if the DLL was load correctly, you can can inside unity confirmMsg(). A messageBox should be displayed

1. To start, call init() inside dll. Assure no one is on top of the balances.
    This will load all the balances inside the dll and start processing the orientation.
2. killBallCheck() will stop the find process of the balances
3. call inside your mainloop getVirtualDirection() to get the orientation of the user
4. pass a vector of 4 floats to getOriSpeed3f(float *b) and get the orientation
5. pass a vector of 4 floats to getSpeed(float *) and get the speed in all directions
    You need both 4 and 5 step to be allow to create motion

You can get messages from the align process via void getLogW(WCHAR *message) call
    make sure you print the content of message during align process continuosly
If you have more than one balance connected, you have to start the align process of the balances with
    enableCorrectBalPosition(); this process will start a procedure that will help adjust the balance locations.
    As in the start the balances get out of order, and our system dependes in the correct balance order to work, follow the process as messages present to you.
    The balances will end in this format after calling and following the steps sequence:

            [2][0]
            [3][1]

    The balances must be in this configuration. All >>the power buttons<< of the balances must be in the same orientation. The align process will ask to you step over the first balance you chose. After to move forward, to the right. After that, it will know the position of all balances and be able to process the orientation of the user on top of whem.

After you code is right and before start using, make sure to have connected all WiiBB in your Wndows via bluetooth asistant.


