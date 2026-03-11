# tcp_chatroom
My first C project.<br>
This program uses unix headers, so make sure you're building this application on a unix based OS.

## Building
You can build both server and client with: <code>make both</code><br>
Or you can make each one separately with either <code>make server_only</code> or <code>make client_only</code><br><br>

## Usage
#### Server
This application uses <code>1234</code> as default port (quite creative, yes?).<br>
To run the server is simples as <code>./server</code>

#### Client
To connect to a server you have to manually insert the ipv4 address you`ll connect<br>
Example <code>./client 192.168.0.1</code><br><br>
#### user-commands:
<code>/rooms (-t theme)</code><br>
<code>/users (-a "all rooms") (-i room_id)</code><br>
<code>/change-room room_id</code><br>
<code>/change-name new_name</code><br>
<code>/create-room name theme</code><br><br>

If in doubt, you can write <code>/help</code> to see all commands or <code>/command --help</code> to see how to use a command.<br><br>

![image_alt](https://github.com/RafaelLimaTarnac/tcp_chatroom/blob/3c4322211631980d6bb362a3b52a04f75f52faa1/print_clients.png)
