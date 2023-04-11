# iTunes-Discord-RPC

Outfled Discord RPC Client for iTunes.

When enabled, this program will connect to your iTunes process & display the current song you're listening to, alongside the artist and album name.
By default, the album cover is not displayed in the RPC status. If you would like to display the album cover image, read below.

## Displaying Album Covers Alongside the RPC Status

To do this, you will need to create a Discord Developer application.
The Discord Developer application will hold all the album cover images for the RPC.
Discord only allows up to 300 assets (images) per application. So depending on the amount of albums in your iTunes library, you may need to make multiple Discord Developer Applications.


### Step 1: Export Each Album Image to a File

Open iTunes.exe

Run albumexport.exe (src/albumexport.exe)

Once completed, albumexport.exe will print the total number of discord applications you will need to create. Complete & repeat step 2 however many times albumexport.exe printed.

### Step 2: Create Discord Developer Application(s)

1: Login to the Discord Developer Portal (https://discord.com/developers/applications)
2: Click the 'New Application' button in the top right
3: Set the name to 'iTunes' and press create
4: In the general information tab, upload the application icon (img/iTunesLogo.jpeg)
5: In the general information tab, scroll down & find the application ID. Click copy and save this for later.







### Step 2: 
