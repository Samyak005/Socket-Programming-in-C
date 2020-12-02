## Problem Statement
The server and client will be two different programs that will be run in different directories.\
For example, let the server directory contains files file1.txt, file2.txt, file3.txt\
The client will create a connection to the server and send requests to download files to the client\
directory. It will write the files into its own directory. Progress should be printed while\
downloading the file.\
Error handling such as requesting missing files must be handled appropriately.\

'client>' 'get <fileName>'
The file if exists at the server side should be downloaded in the client directory else appropriate\
error must be displayed.

'client>' 'get <file1> <file2> <file3>'
Multiple files requested should be downloaded in the client directory.\

'client>' 'exit'
connection with the server is closed.\

File size can be greater than 1 GB.\

## Instructions
1. Open folder client and server
2. Open one Terminal tab in client folder and the other in server folder
3. Run gcc client.c -o client in client tab and gcc server.c -o server in server tab
4. Run ./server followed by ./client

### Sample Output:
./client

```
>>> get A.txt
Request for A.txt...
file size 1200000000
Completed - Total File Size 1200000000 Total Written 1200000000
A.txt File download completed
>>> get file1.txt
Request for file1.txt...
file size 64
Completed - Total File Size 64 Total Written 64
file1.txt File download completed
>>> ^C
```

./server 
```
Sending filename A.txt
File sent
Sending filename file1.txt
File sent
```

## Code:
### 'client.c' : 

getfilename() function
first sends file name to the server and server sends back 'ERROR' if file not found in its directory, otherwise sends the file size.
If server sends ERROR print error message(file not found).

buffer size - 1024
keep reading contents from file in server directory and keep writing. Handled boundary cases like file size exact multiple of 1024 and in the end when size left is less than 1024. 
File size has been passed as FILESIZE followed by the number and is tokenised using strtok to extract the exact number.

splitline() function 
splits the command line input containing multiple files after get command using strtok

main function while(1) loop -> CLI

### 'server.c':
Keep checking for exit code sent, otherwise keep reading from file and sending to socket connection.