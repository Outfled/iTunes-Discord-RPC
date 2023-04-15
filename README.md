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
    
## Errors

### 2147746132
	The most common cause is that iTunes.exe is running as admin and iTunesDiscordRPC.exe is not running as administrator.
	Or vice versa, iTunesDiscordRPC.exe is running as administrator while iTunes.exe is not

### Discord not displaying status while music is playing on iTunes

	Open discord, on the bottom left, click User Settings then select Activity Privacy on the bottom left.
	Make sure 'Display current activity as a status message.' is checked.
	Additionally, make sure Discord is not running as administrator. Unless iTunes.exe AND iTunesDiscordRPC.exe are running as administrator
	
## Displaying Album Covers Alongside the RPC Status

To do this, you will need to create a Discord Developer application, or multiple, depending on the amount of albums in your iTunes library.

The Discord Developer application will hold all the album cover images for the RPC.

Discord only allows up to 300 assets (images) per application.
So depending on the amount of albums in your iTunes library, you may need to make multiple Discord Developer Applications.


### Step 1: Export Each Album Image to a File

-Make sure you have iTunes running

-Start playing _any_ song that you have downloaded

-Run albumexport.exe (src/albumexport.exe)

-Once completed, which can take a bit of time depending on how big your library is, albumexport.exe will print the total number of discord applications you will need to create. Complete & repeat step 2 however many times albumexport.exe printed.

### Step 2: Create Discord Developer Application(s)

-Login to the Discord Developer Portal (https://discord.com/developers/applications)

-Click the 'New Application' button in the top right

-Set the name to 'Apple Music 1'

	Note: if this not the first application you're creating (for this), then change the 1 to the next incremental value.
	Example: 
		If this is the second application, the name should be 'Apple Music 2'.
		Then the third application should be 'Apple Music 3', and so on and so forth
	
This will be the RPC status display name.

![Capture](https://user-images.githubusercontent.com/55547083/232207910-e640079b-b4b8-49af-94d9-005fbb538f92.PNG)

You will change the name later. For now, keep it as instructed.

-Scroll down & find the application ID. Press copy and save this for later.


### Step 3: Upload the Album Images to the Discord Developer Application(s)

-Select the current discord application from https://discord.com/developers/applications

-On the left, navigate to Rich Presence -> Art Assets

-Press Add Image(s)

-Navigate to the directory that you ran albumexport.exe

-There should be a folder with the same name as the _current_ application you're uploading to

-Open the corresponding foler and select all the & upload.

-Press the 'Save Changes' button at the bottom

-Wait for all files to upload.

If you get a message saying you are being rate limited:
    
    Wait for the rest of the files to finish.
    Wait an additional few seconds.
    Press the 'Save Changed' button again
    Repeat until the rate limit message is gone.

If you get a message saying 'Minimum Size: 512x512':

    Press the red X next to the file name.
    Press enter.
    You can choose to upload a bigger image for the file if you want.
    Make sure if you do upload a different image, the asset name is the exact same as the original one
    
There may be a bug where after you are done uploading, all the images disappear. Don't worry, its all visual; give it some time to completely upload the images

-Additionally, you can upload a cover image at the top of the page, which will be used in the event that the iTunes album is not found in the application assets.

	If you choose to upload an image, you can use the iTunes logo image located in img/iTunes-Logo.png 
	You only need to select the cover image for the first Discord Application.
	
-Repeat step 3 for each application created. Move to step 4 when finished.

### Step 4: Replace the Header & Source Files

-Navigate to the directory that you ran albumexport.exe

-There should be two files:

    new_AlbumAssets.h
    new_DiscordStatus.cpp
    
-Copy new_AlbumAssets.h into inlude/

-Copy new_DiscordStatus.cpp into src/

-Delete the file include/AlbumAssets.h

-Rename new_AlbumAssets.h to AlbumAssets.h

-Delete the file src/DiscordStatus.cpp

-Rename new_DiscordStatus.cpp to DiscordStatus.cpp

### Step 5: Define the Discord Developer Application ID(s)

-Open the new include/AlbumAssets.h file (don't open w/Visual Studio; use a text editor. VS wont initially be able to read AlbumAssets.h for some reason)

-Delete the following line:

    // TODO : DEFINE RPC_ASSET{index}_APPLICATION_ID FOR EACH DISCORD APPLICATION CREATED\n\n"
       
-Repeat the upcoming steps for *each* discord application you created:

    Type the following into the current line:
    #define RPC_ASSET{index}_APPLICATION_ID "{APPLICATION_ID}"
    
    Replace {index} with the number in the current applications name.
    Examples:
    	if the current application is named 'Apple Music 1', the line should be:
    		#define RPC_ASSET{index}_APPLICATION_ID "{APPLICATION_ID}"
	if the current application is named 'Apple Music 2', the line should be:
		#define RPC_ASSET{index}_APPLICATION_ID "{APPLICATION_ID}"
	So on and so forth.
	
    Replace {APPLICATION_ID} with the application ID of the current application
    Make sure the application id is wrapped in quotes
    Example:
    	If my application ID was 237121248742, and the application name was 'Apple Music 1', the line would look like this
		#define RPC_ASSET1_APPLICATION_ID "237121248742"
        If my application ID was 127583291293, and the application name was 'Apple Music 2', the line would look like this
		#define RPC_ASSET1_APPLICATION_ID "127583291293"
	So on and so forth.
	
-Sace & close include/AlbumAssets.h when completed

### Step 5: Paste the application id(s) into DiscordStatus.cpp

-Open the new src/DiscordStatus.cpp

-Go to line 67, you should see the following line:

-Repeat the following steps for each discord application created

    Find the block of code in src/DiscordStatus.cpp (the first block should be around line 67) that is equal to the code below:
    
	for ( int i = 0; i < ARRAYSIZE( rgszAlbumNames_1 ) && lpszApplicationID == NULL; ++i )
	{
		if (0 == strcmp( rgszAlbumNames_1[i], Album ) ) {
			/* TODO : REPLACE COMMENT WITH CORRESPONDING APPLICATION ID */
		}
	}

    Delete '/* TODO : REPLACE COMMENT WITH CORRESPONDING APPLICATION ID */' from the block.
    Type:
    	lpszApplicationID = 
    Do not press enter, you now have to enter the corresponding definition name.
    In include/AlbumAssets.h, you had to create definitions for each application ID.
    Type the definition name that has the current discord applications ID.
    Example: 
	If this was the FIRST block, the line would now look like this:
		lpszApplicationID = RPC_ASSET1_APPLICATION_ID;
	If this was the second block, the line would now look like this:
		lpszApplicationID = RPC_ASSET2_APPLICATION_ID;
	So on and so forth.
	
    Again, to clarify, there should be the same amount of blocks-of-code as the number of definitions you had to create in include/AlbumAssets.h
    You need to go to EACH block and follow the steps above.
    
Examples:

 If I have 2 discord applications, the ORIGINAL src/DiscordStatus.cpp would look like this:
    
	for ( int i = 0; i < ARRAYSIZE( rgszAlbumNames_1 ) && lpszApplicationID == NULL; ++i )
	{
		if (0 == strcmp( rgszAlbumNames_1[i], Album ) ) {
			/* TODO : REPLACE COMMENT WITH CORRESPONDING RPC_ASSET{index}_APPLICATION_ID */
		}
	}
	for ( int i = 0; i < ARRAYSIZE( rgszAlbumNames_2 ) && lpszApplicationID == NULL; ++i )
	{
		if (0 == strcmp( rgszAlbumNames_2[i], Album ) ) {
			/* TODO : REPLACE COMMENT WITH CORRESPONDING RPC_ASSET{index}_APPLICATION_ID */
		}
	}
	
After completing all steps, it would now look like this:
    
   	for ( int i = 0; i < ARRAYSIZE( rgszAlbumNames_1 ) && lpszApplicationID == NULL; ++i )
	{
		if (0 == strcmp( rgszAlbumNames_1[i], Album ) ) {
			lpszApplicationID = RPC_ASSET1_APPLICATION_ID;
		}
	}
	for ( int i = 0; i < ARRAYSIZE( rgszAlbumNames_2 ) && lpszApplicationID == NULL; ++i )
	{
		if (0 == strcmp( rgszAlbumNames_2[i], Album ) ) {
			lpszApplicationID = RPC_ASSET2_APPLICATION_ID;
		}
	}
	
*_Do not forget the semicolon at the end of the line._

### Step 6: Rebuild the project

-On the top of visual studio, set the configuration options to Release.
-On the top of Visual Studio, click Build -> Build Solution
-If successful, the exe will be in build/iTunesDiscordRPC.exe

## Questions/Concerns/Help
If you have any questions or concerns or need help, you can add my discord:

    Outfled#5243
or

    https://discord.com/users/902777692477333504
    
