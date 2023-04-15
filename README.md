# iTunes-Discord-RPC

Outfled Discord RPC Client for iTunes (Windows 10)

When enabled, this program will connect to your iTunes process & display the current song you're listening to, alongside the artist and album name.
By default, the album cover is not displayed in the RPC status. If you would like to display the album cover image, keep reading below.

## Features

    Minimizes to tray
    Detects when iTunes and or Discord applications are not running to avoid crashing
    Can be disabled without having to close the program
    
## Requirements

Visual Studio:

https://visualstudio.microsoft.com/vs/community/


C++ MFC:

Install from Visual Studio Installer -> Modify -> Individual Components -> C++ MFC for latest {version} build tools ({architect})


iTunes (Obviously):

https://support.apple.com/downloads/itunes


Discord (Obviously):

https://discord.com/download
    
## Displaying Album Covers Alongside the RPC Status

To do this, you will need to create a Discord Developer application.
The Discord Developer application will hold all the album cover images for the RPC.
Discord only allows up to 300 assets (images) per application. So depending on the amount of albums in your iTunes library, you may need to make multiple Discord Developer Applications.


### Step 1: Export Each Album Image to a File

-Make sure you have iTunes running

-Run albumexport.exe (src/albumexport.exe)

-Once completed, which can take a bit of time depending on how big your library is, albumexport.exe will print the total number of discord applications you will need to create. Complete & repeat step 2 however many times albumexport.exe printed.

### Step 2: Create Discord Developer Application(s)

-Login to the Discord Developer Portal (https://discord.com/developers/applications)

-Click the 'New Application' button in the top right

-Set the name to 'iTunes' and press create (duplicate application names are allowed). This will be the RPC status display name.

![image](https://user-images.githubusercontent.com/55547083/231023853-b35955b0-ce14-4874-a281-2b0e083dc1cf.png)

-In the general information tab, upload the application icon (img/iTunesLogo.jpeg). This image will be the default image used if the Discord client tries to display an asset image that does not exist.

-In the general information tab, scroll down & find the application ID. Press copy and save this for later.


### Step 3: Upload the Album Images to the Discord Developer Application(s)

-On the left, navigate to Rich Presence -> Art Assets

-Press Add Image(s)

-Navigate to the directory that you ran albumexport.exe.

    Back in step 1, if albumexport.exe printed '1':
      Open the folder named 'iTunes-1' and select all the files.
    
    If albumexport.exe printed a number bigger than 1:
      Open the corresponding 'iTunes-{index}' folder.
      Example:
        if this is the first application that you are uploading to, open the folder 'iTunes-1'
        If this is the second application that you are uploading to, open the folder 'iTunes-2'
        So on and so forth
        
      Select all the files in the folder & upload.
      Do not change the names of any of the files

-Wait for all files to upload.
If you get a message saying you are being rate limited:
    
    Wait for the rest of the files to finish.
    Wait an additional few seconds.
    Go to the top and select the first album asset.
    Press enter and wait for upload(s) to complete.
    Repeat until the rate limit message is gone.

If you get a message saying the file image is too small:

    Press the red X next to the file name.
    You can choose to upload a bigger image for the file if you want.
    Make sure if you do upload a different image, the asset name is the exact same as the original one
    
-Repeat step 3 for each application created. Move to step 4 when finished.

### Step 4: Replace the Header & Source Files

-Navigate to the directory that you ran albumexport.exe
-There should be two files:

    new_AlbumAssets.h
    new_DiscordStatus.cpp
    
-Copy new_AlbumAssets.h into src/
-Copy new_DiscordStatus.cpp into src/
-Delete the file src/AlbumAssets.h
-Rename new_AlbumAssets.h to AlbumAssets.h
-Delete the file src/DiscordStatus.cpp
-Rename new_DiscordStatus.cpp to DiscordStatus.cpp

### Step 5: Define the Discord Developer Application ID(s)

-Open the new AlbumAssets.h file (don't open w/Visual Studio; use a text editor. VS wont initially be able to read AlbumAssets.h for some reason)
-Delete the following line:

    /* TODO : DEFINE RPC_ASSET{index}_APPLICATION_ID FOR EACH DISCORD APPLICATION CREATED */
       
-Repeat the upcoming steps for *each* discord application you created:

    Type the following into the current line:
    #define RPC_ASSET{index}_APPLICATION_ID "{APPLICATION_ID}"
        
    If this is the first definition, replace {index} with 1, like so: RPC_ASSEST1_APPLICATION_ID
    If this is NOT the first definition, replace {index} with the previous line {index} + 1.
    For example, if i have 2 applications, the lines would look like so:
        #define RPC_ASSET1_APPLICATIONID "{APPLICATION_ID}"
        #define RPC_ASSET2_APPLICATIONID "{APPLICATION_ID}"
            
    Next, replace {APPLICATION_ID} with the corresponding Discord application id.
    If you had multiple application, the {APPLICATION_ID} for each definition line should always be different.
    Full examples:
        Lets say my application id is 185392184 and this is my first #define line. My line would look like this:
            #define RPC_ASSET1_APPLICATION_ID "185392184"
        Lets say my next application id is 91823652 and this is my second #define line:
            #define RPC_ASSET2_APPLICATION_ID "91823652"
        So on and so forth.
            
        Make sure the application id is wrapped in quotes like the example above.
        
### Step 5: Paste the application id(s) into DiscordStatus.cpp

-Open the new DiscordStatus.cpp
-Go to line 68, you should see the following line:

    for ( int i = 0; i < ARRAYSIZE( rgszAlbumNames_1 ) && lpszApplicationID == NULL; ++i )
	{
		if (0 == strcmp( rgszAlbumNames_1[i], Album ) ) {
		    /* TODO : REPLACE COMMENT WITH CORRESPONDING APPLICATION ID */
		}
	}

-Depending on how many discord applications you had to create, there may be more similar lines right underneath the lines above.
-Back in step 4, you had to create definitions (#define) in AlbumAssets.h for each discord application.
-Starting from the first application ID, replace

    // TODO : DEFINE RPC_ASSET{index}_APPLICATION_ID FOR EACH DISCORD APPLICATION CREATED
        
With:
	lpszApplicationID = {APPLICATION_ID_DEFINITION_NAME};
Replace {APPLICATION_ID_DEFINITION_NAME} with the name of the definition for the corresponding application ID
-For example, if this was the first for-loop, the line would look like this:
	lpszApplicationID = RPC_ASSET1_APPLICATION_ID;
-If it was the second for-loop, the line would look like this:
	lpszApplicationID = RPC_ASSET2_APPLICATION_ID;
-So on and so forth
-Again, to clarify, if you created multiple definitions/discord-applications, there will be multiple lines you have to edit.
-Just look for "// TODO : DEFINE RPC_ASSET{index}_APPLICATION_ID FOR EACH DISCORD APPLICATION CREATED" and make sure you edit each one.
Going in order of the corresponding application id definition from AlbumAssets.h

### Step 6: Rebuild the project
-On the top of visual studio, set the configuration options to Release.
-On the top of Visual Studio, click Build -> Build Solution
-If successful, the exe will be in ..\x64\Release\iTunes Discord RPC.exe

## Questions/Concerns/Help
If you have any questions or concerns or need help, you can add my discord:

    Outfled#5243
or

    https://discord.com/users/902777692477333504
    
