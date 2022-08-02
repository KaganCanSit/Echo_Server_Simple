/* 
 * File:   main.cpp
 * Author: kagancansit
 * Information Source: https://www.youtube.com/watch?v=CFe5LQOPdfk
 * Created on August 2, 2022, 10:34 AM
 * 
 * SERVER
 * g++ -std=c++11 echo_server.cpp -o server
 * ./server 5000
 * 
 * CLIENT
 * nc localhost 5000
 */

/*
 * Echo Sunucusu
 * 1 -> Socket  2 -> Bind   3 -> Listen
 * 4 -> Accept  5 -> Recv   6 -> Send
 * 7 -> Goto 5
 */

#include <iostream>
#include <cstdlib>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <strings.h>

using namespace std;

int main(int argc, char** argv) {

    //Sunucu düzenlemesinde argv[0] her zaman yürütülebilir dosyalar için ayrılır. Sunucu yapısını korumak için yürütülebilir dosya, port numaraları parametreleri kontrol ediyoruz.
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <Port Number>" << endl;
        return 0;
    }

    //Port numarası bağlantı ve iletişim için gereklidir. 
    int portNumber = atoi(argv[1]);


    /* 
     * Socket
     * 1 -> Domain = AF_UNIX (Unix dosyası), AF_INET (IPv4), AF_INET6 (IPv6)  
     * 2 -> Type = SOCK_STREAM (TCP), SOCK_DGRAM (UDP), SOCK_RAW
     */
    int socketNum = socket(AF_INET, SOCK_STREAM, 0);


    // Bind -- Bağlantı adresi üzeirnde nereye bağlanacağımı bilmemiz adres vb. bilgilerin bir yapı içerisinde tutmamız gerekir.
    struct sockaddr_in address;
    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET; //(IPv4)
    address.sin_port = htons(portNumber); //Htons bilgisayarlar arasında byte önceliklendirmesini haberleşme için düzenler. Sayıların en önemli bayt ilk olacak şekilde ağ bayt sırasına göre bellekte saklanmasını sağlar.

    int bindValue = bind(socketNum, (struct sockaddr *) &address, sizeof (address));
    if (bindValue < 0) {
        perror("- Bağlantı sağlanamadı.\n");
        return 1;
    }

    //Listen
    int listenValue = listen(socketNum, 1);
    if (listenValue < 0) {
        perror("Dineleme Başarısız. \n");
        return 1;
    } else
        cout << "\nSunucu " << portNumber << " numaralı portu dinliyor." << endl;



    //Accept
    struct sockaddr_in remote_address;
    memset(&address, 0, sizeof (address));
    socklen_t remote_addrlen = sizeof (address);


    //Bağlantu kurmak isteyen Client hakkındaki verileri accept verisi ile bir socket adresi struct'ı içerisine tanımlıyoruz.
    cout << "\nBağlantı bekleniyor." << endl;
    int client_socket = accept(socketNum, (struct sockaddr *) &remote_address, &remote_addrlen);
    if (client_socket < 0) // 1 Hata - 0 Client'in bağlantısı kesildi.
    {
        perror("Bağlantı kurulamadı.");
        return 1;
    } else if (client_socket == 0) {
        perror("Client'in bağlantısı kesildi veya sonlandırıldı.");
        return 1;
    }


    string client_ip = inet_ntoa(remote_address.sin_addr);
    int remote_port = ntohs(remote_address.sin_port); //htons fonksiyonun tersini gerçekleştirir.
    cout << "Yeni bağlantı sağlandı. IP: " << client_ip << " Port:" << remote_port << endl;

    int BUFF_LEN = 1024; //Ara Bellek
    char buffer[BUFF_LEN];

    while (1) {
        memset(buffer, 0, BUFF_LEN); //Client'in kullandığı ara belleği her bağlantı öncesi temizliyoruz.
        //Recv
        int bytes_received = recv(client_socket, buffer, BUFF_LEN - 1, 0);
        if (bytes_received < 0) {
            perror("Veri alınamadı.\n");
            return 1;
        }
        if (bytes_received == 0) {
            cout << "IP: " << client_ip << " Port :" << remote_port << "olan bağlantı sonlandırıldı." << endl;
            break;
        }
        if (buffer[bytes_received - 1] == '\n') {
            buffer[bytes_received - 1] = 0;
        }
        cout << "Client Mesajı: \"" << buffer << "\"" << endl;

        //Send
        string response;
        response = "Gönderilme Bilgileri-> Adres: " + client_ip + " Port Numarası: " + to_string(remote_port) + "\nMesajın: " + string(buffer) + "\n";
        int bytes_sent = send(client_socket, response.c_str(), response.length(), 0);
        if (bytes_sent < 0) {
            perror("Gönderilemedi.");
            return 1;
        }

    }

    cout << "Socket kapatılıyor." << endl;
    shutdown(client_socket, SHUT_RDWR);

    return 0;
}

