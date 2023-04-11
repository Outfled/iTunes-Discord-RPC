# iTunes-Discord-RPC

Outfled Discord RPC Client for iTunes.

When enabled, this program will connect to your iTunes process & display the current song you're listening to, alongside the artist and album name.
By default, the album cover is not displayed in the RPC status. If you would like to display the album cover image, read below.

## Displaying Album Covers Alongside the RPC Status

To do this, you will need to create a Discord Developer application.
The Discord Developer application will hold all the album cover images for the RPC.
Discord only allows up to 300 assets (images) per application. So depending on the amount of albums in your iTunes library, you may need to make multiple Discord Developer Applications.


### Step 1: Export Each Album Image to a File

-Make sure you have iTunes running

-Run albumexport.exe (src/albumexport.exe)

-Once completed, albumexport.exe will print the total number of discord applications you will need to create. Complete & repeat step 2 however many times albumexport.exe printed.

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
-Rename new_DiscordStatus.cpp to DiscordStatus.cpp

### Step 4: Build the Project
-Open the solution (.sln) file in Visual Studio

-Set the configuration to release x64

-On the top of the window, press Build -> Build Solution

-The exe file should be in the x64/ folder.

## Help/Questions

If you have any questions or need any help, you can add me on Discord:

   Outfled#5243
        
If you cant find my account you can also add me through the link(s) below:

http://discordapp.com/users/902777692477333504

