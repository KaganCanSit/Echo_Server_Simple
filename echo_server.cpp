/* 
 * File: echoserver.cpp || Author: kagancansit
 * Information Sources: 
 *          https://www.youtube.com/watch?v=CFe5LQOPdfk
 *          https://www.yusufsezer.com.tr/c-cpp-soket/
 * Created on August 2, 2022, 10:34 AM
 */

/*
 * SERVER
 * g++ -std=c++11 echo_server.cpp -o server
 * ./server 5000
 * 
 * CLIENT
 * nc localhost 5000
 */

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <string>

using namespace std;

int main(int argc, char** argv) {

    /*
     * Sunucu düzenlemesinde argv[0] her zaman yürütülebilir dosyalar için ayrılır.
     * Sunucu yapısını korumak için argv[0] kullanılmamasını kontrol ediyoruz.
     * Kontrolü sağlayıp, port numarasını parametre olarak alıyoruz.
     */
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <Port Number>" << endl;
        return 0;
    }

    //Port numarasının alınması. 
    int portNumber = atoi(argv[1]);

    //Socket Fonk. -> 1.Par: AF_INET (IPv4), AF_INET6 (IPv6)  // 2.Par -> SOCK_STREAM (TCP), SOCK_DGRAM (UDP)
    int socketNum = socket(AF_INET, SOCK_STREAM, 0);


    // Bind -- Sunucunun dinleme yapabilmesi ve iletişime geçebilmesi için port tanımı.
    struct sockaddr_in address; //Socket bilgilerini tanımlamamış için hazır struct yapısı.
    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET; //(IPv4)
    address.sin_port = htons(portNumber); //Htons bilgisayarlar arasında byte önceliklendirmesini haberleşme için düzenler.

    int bindValue = bind(socketNum, (struct sockaddr *) &address, sizeof (address));
    if (bindValue < 0) {
        perror("- Bağlantı sağlanamadı.\n");
        return 1;
    } else
        cout << "\nBaşarıyla socket'e bağlanıldı." << endl;

    //Listen
    int listenValue = listen(socketNum, 1); //Hangi Port Numarası, kaç kişi için dinleneceğini belirtiyoruz.
    if (listenValue < 0) {
        perror("Socket dinleme başarısız. \n");
        return 1;
    } else
        cout << "\nSunucu " << portNumber << " numaralı portu dinliyor." << endl;



    //Accept
    struct sockaddr_in remote_address;
    memset(&remote_address, 0, sizeof (remote_address));
    socklen_t remote_addrlen = sizeof (remote_address);


    //Bağlantı kurmak isteyen Client hakkındaki verileri accept verisi ile bir socket adresi struct'ı içerisine tanımlıyoruz.
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

    //------------------------
    string response = "Başarıyla sunucuya bağlandınız.\nMesajınız: ";
    int bytes_sent = send(client_socket, response.c_str(), response.length(), 0);
    if (bytes_sent < 0) {
        perror("Gönderilemedi.");
        return 1;
    }
    //------------------------




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
            cout << "IP: " << client_ip << " Port :" << remote_port << " olan bağlantı sonlandırıldı." << endl;
            break;
        }
        if (buffer[bytes_received - 1] == '\n') {
            buffer[bytes_received - 1] = 0;
        }
        cout << "Client Mesajı: \"" << buffer << "\"" << endl;

        //Send
        string response;
        response = "\nMerhaba Client! {Adres Bilgilerin -> IP: " + client_ip + " Port Numarası: " + to_string(remote_port) + "}\nBana iletmiş olduğun mesajın: " + string(buffer) + "\nMesajını aldım.";
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

