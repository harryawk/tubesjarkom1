#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

char* parse_command(int tipe) { 
	// tipe 1 = total memory
	// tipe 2 = used memory
	// tipe 3 = free memory

// masih bug
	char *cmd = "free";

	#define BUFSIZE 128
  char buf[BUFSIZE];
  char memtotal[BUFSIZE];
  char memfree[BUFSIZE];
  char memused[BUFSIZE];

  bzero(memtotal, BUFSIZE);
  bzero(memfree, BUFSIZE);
  bzero(memused, BUFSIZE);

  FILE *fp;

  if ((fp = popen(cmd, "r")) == NULL) {
      printf("Error opening pipe!\n");
      return -1;
  }
      
      // Do whatever you want here...

  int iterasi = 1;
  while(fgets(buf, BUFSIZE, fp) != NULL) {

      if (iterasi == 2) {
      		printf("masuk\n");
          int i = 0;
          while (buf[i] != ' ') {
              i++;
          } // end of "Mem:"
          while (buf[i] == ' ') {
              i++;
          } // total memory
          int j = 0;
          while (buf[i] != ' ') {
              if (tipe == 1) {
              	memtotal[j] = buf[i];
              }
              i++;
              j++;
          } // end of total memory
          j = 0;
          while (buf[i] == ' ') {
              i++;
          } // used memory
          while (buf[i] != ' ') {
              if (tipe == 2) {
              	memused[j] = buf[i];
              }
              i++;
              j++;
          } // end of used memory
          j = 0;
          while (buf[i] == ' ') {
              i++;
          } // free memory
          while (buf[i] != ' ') {
          		if (tipe == 3) {
              	memfree[j] = buf[i];
              }
              i++;
              j++;
          } // end of free memory
          if (tipe == 1) {
		      	return memtotal;
		      } else if (tipe == 2) {
		      	return memfree;
		      } else if (tipe == 3) {
		      	return memused;
		      }
      }
      iterasi++;
      
      
  }

  if(pclose(fp))  {
      printf("Command not found or exited with error status\n");
      return -1;
  }
}

int main()
{
 	// buffer yang menyimpan string dari client
  char str[100];
  char buffer[1024];

  // File descriptor untuk listen and communication, diterima oleh socket
  // FD listen untuk respon dari client saat membentuk koneksi
  // FD communication untuk mengirim pesan ke client
  int listen_fd, communication_fd;

	// servaddr adalah struktur socket
  struct sockaddr_in servaddr;

  // nilai dari struktur file deskriptor yang akan diterima server saat ada client yang hendak membentuk koneksi
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Menginisialisasi struct servaddr dengan 0, sama dengan memset(&servaddr, 0, sizeof(servaddr))
  bzero( &servaddr, sizeof(servaddr));

	// Assign nilai address server
	// sin_family = addressing scheme
	// sin_addr.s_addr = IP yang dapat membentuk koneksi dengan server
	// sin_port = port dimana server dapat dikontak
	// htons() mengubah nomor port ke Big Endian short integer.
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(22000);

  // Prepare untuk listen dari client di address server diatas
  bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  // Mulai listening request koneksi, bila ada request pembentukan koneksi, listen_fd akan dikembalikan
  listen(listen_fd, 10);

  // Menunggu dan accept koneksi saat listen_fd direturn, lalu communication_fd digunakan untuk melakukan komunikasi antarclient-server
  communication_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

  strcpy(str,"# munin node at MyComputer");
    
  while(1)
  {
		// Mengisi buffer untuk komunikasi dengan 0
	  bzero(str, 100);
  	  // write(communication_fd, str, strlen(str)+1);
	  // bzero(str, strlen(str));
	  // Membaca request dari client
	  read(communication_fd,str,100);	  
	  printf("cap = %d\n", strncmp(str,"cap",strlen("cap")));

	  if (strncmp(str,"cap",strlen("cap")) == 0) { // cap
	  	strcpy(str,"cap multigraph dirtyconfig");
	  	printf("%s\n", str);
	  	printf("masuk 1\n");
	  } 
	  else if (strncmp(str,"nodes",strlen("nodes")) == 0) { // nodes
	  	strcpy(str,"MyComputer");
	  	printf("%s\n", str);
	  	printf("masuk 2\n");
	  } 
	  else if (strncmp(str,"list MyComputer",strlen("list MyComputer")) == 0) { // list MyComputer
	  	strcpy(str, "memory");
	  	printf("%s\n", str);
	  	printf("masuk 3\n");
	  } 
	  else if (strncmp(str,"config memory",strlen("config memory")) == 0) { // config memory
	  	strcpy(str, parse_command(1));
	  	write(communication_fd, str, strlen(str));
	  	bzero(str, 100);
	  	strcpy(str, "total memory");
	  	write(communication_fd, str, strlen(str));
	  	continue;
	  } 
	  else if (strncmp(str,"fetch memory",strlen("fetch memory")) == 0) { // fetch memory
	  	strcpy(str, "fetch memory");
	  	continue;
	  } 
	  else if (strncmp(str,"version",strlen("version")) == 0) { // version
	  	strcpy(str, "TropicalIsland v0.0");
	  } 
	  else if (strncmp(str,"quit", strlen("quit")) == 0) { // quit
	  	strcpy(str, "Connection closed by foreign host");
	  }
	  else { // otherwise
	  	strcpy(str, "# Unknown command. Try cap, list, nodes, config, fetch, version or quit");
	  	printf("%s\n", str);
	  	printf("masuk 4\n");
	  }
	  // Mengirim response ke client
	  write(communication_fd, str, strlen(str));
  }
}
