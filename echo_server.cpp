/* 
 * File: echoserver.cpp || Author: kagancansit
 * Information Sources: https://www.youtube.com/watch?v=CFe5LQOPdfk --- https://www.yusufsezer.com.tr/c-cpp-soket/
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
    
    //Haberleşme için server'ın kullacığı port numarasının alınması.
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <Port Number>" << endl;
        return 0;
    } 
    int portNumber =  atoi(argv[1]);
    
    //Socket Fonk. -> 1.Par: AF_INET (IPv4), AF_INET6 (IPv6)  // 2.Par -> SOCK_STREAM (TCP), SOCK_DGRAM (UDP) // 3.Par -> Transfer Prot.
    //Habrleşme için port tanımı.
    int socketNum = socket(AF_INET, SOCK_STREAM, 0);   
    
    // Sunucu ve client için socket ip ilişkilendirilmesi.
    struct sockaddr_in server_address, client_address; 
    memset(&server_address, 0, sizeof (server_address));
    server_address.sin_family = AF_INET;           //(IPv4)
    server_address.sin_port = htons(portNumber);   //Htons bilgisayarlar arasında byte önceliklendirmesini haberleşme için düzenler.
    
    memset(&client_address, 0, sizeof (client_address));
    socklen_t remote_addrlen = sizeof (client_address);
    
    //İçerik aktarımı için ara bellek tanımlanması
    int BUFF_LEN = 1024;
    char buffer[BUFF_LEN];
    
    //--BIND--
    if ((bind(socketNum, (struct sockaddr *) &server_address, sizeof (server_address)))< 0) 
    {
        perror("- Bağlantı sağlanamadı.\n");
        return 1;
    } 
    else
        cout << "\nBaşarıyla socket'e bağlanıldı." << endl;


    //--LISTEN--
    //Hangi Port Numarası, kaç kişi için dinleneceğini belirtiyoruz. Fazlaa istek sonucu parametre sonrası kabul edilmez.
    if ((listen(socketNum, 1)) < 0) 
    {
        perror("Socket dinleme başarısız. \n");
        return 1;
    } else
        cout << "\nSunucu " << portNumber << " numaralı portu dinliyor. Bağlantı bekleniyor." << endl;


    //--ACCEPT--
    //Bağlantı kurmak isteyen Client hakkındaki verileri accept verisi ile bir socket adresi struct'ı içerisine tanımlıyoruz.
    int client_socket = accept(socketNum, (struct sockaddr *) &client_address, &remote_addrlen);
    if (client_socket < 0) // 1 Hata - 0 Client'in bağlantısı kesildi.
    {
        perror("Bağlantı kurulamadı.");
        return 1;
    }
    else if (client_socket == 0) 
    {
        perror("Client'in bağlantısı kesildi veya sonlandırıldı.");
        return 1;
    }
    
    string client_ip = inet_ntoa(client_address.sin_addr); //inet_ntoa -> Bilgisayar adresini IPv4 olarak ondalık diziye dönüştürür.
    int remote_port = ntohs(client_address.sin_port); //htons fonksiyonun tersini gerçekleştirir.
    cout << "Yeni bağlantı sağlandı. IP: " << client_ip << " Port:" << remote_port << endl;

    while (1) {
        memset(buffer, 0, BUFF_LEN); //Client'in kullandığı ara belleği her bağlantı öncesi temizliyoruz.
        //--RECV--
        int bytes_received = recv(client_socket, buffer, BUFF_LEN - 1, 0);
        if (bytes_received < 0) 
        {
            perror("Veri alınamadı.\n");
            return 1;
        }
        if (bytes_received == 0) 
        {
            cout << "IP: " << client_ip << " Port :" << remote_port << " olan bağlantı sonlandırıldı." << endl;
            break;
        }
        if (buffer[bytes_received - 1] == '\n') 
        {
            buffer[bytes_received - 1] = 0;
        }
        cout << "Client Mesajı: \"" << buffer << "\"" << endl;

        //--SEND--
        string response;
        response = "\nMerhaba Client! {Adres Bilgilerin -> IP: " + client_ip + " Port Numarası: " + to_string(remote_port) +
                "}\nBana iletmiş olduğun mesajın: " + string(buffer) + "\nMesajını aldım.\n\n";
        if ((send(client_socket, response.c_str(), response.length(), 0))< 0) 
        {
            perror("Cevap gönderilemedi.");
            return 1;
        }
    }
    
    cout << "Socket kapatılıyor." << endl;
    shutdown(client_socket, SHUT_RDWR);
    
    return 0;
}