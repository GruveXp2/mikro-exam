#include "Menu.h"
#include "NewsFeed.h"
#include "Network.h"
#include <cstdio>

NewsFeed::NewsFeed(Menu* menu, int& buttonFlags, NetworkInterface* network)
    : View(menu, buttonFlags), network(network), update_thread(osPriorityNormal) {
        update_thread.start(callback(this, &NewsFeed::thread_task));
    }

const char* SSL_CA_PEM2 = 
"-----BEGIN CERTIFICATE-----\n"
"MIIDAjCCAomgAwIBAgINAe5fIpVCSQX5AZGo3DAKBggqhkjOPQQDAzBQMSQwIgYD\n"
"VQQLExtHbG9iYWxTaWduIEVDQyBSb290IENBIC0gUjUxEzARBgNVBAoTCkdsb2Jh\n"
"bFNpZ24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMTgxMTIxMDAwMDAwWhcNMjgx\n"
"MTIxMDAwMDAwWjBQMQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBu\n"
"di1zYTEmMCQGA1UEAxMdR2xvYmFsU2lnbiBFQ0MgT1YgU1NMIENBIDIwMTgwdjAQ\n"
"BgcqhkjOPQIBBgUrgQQAIgNiAATDoRGNZSPhluG7q6bQA11PTe0ZD/xx44QlFam1\n"
"BM4eLeN+wfgwalsbkjzARCM9si/fnQeKNtKAlgNmNOHTmV3VfwGbocj6+22HVWZu\n"
"VeX/VeIGoWh1u7Lja/NDE7RsXaCjggEpMIIBJTAOBgNVHQ8BAf8EBAMCAYYwEgYD\n"
"VR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUWHuOdSr+YYCqkEABrtboB0ZuP0gw\n"
"HwYDVR0jBBgwFoAUPeYpSJvqB8ohREom3m7e0oPQn1kwPgYIKwYBBQUHAQEEMjAw\n"
"MC4GCCsGAQUFBzABhiJodHRwOi8vb2NzcDIuZ2xvYmFsc2lnbi5jb20vcm9vdHI1\n"
"MDYGA1UdHwQvMC0wK6ApoCeGJWh0dHA6Ly9jcmwuZ2xvYmFsc2lnbi5jb20vcm9v\n"
"dC1yNS5jcmwwRwYDVR0gBEAwPjA8BgRVHSAAMDQwMgYIKwYBBQUHAgEWJmh0dHBz\n"
"Oi8vd3d3Lmdsb2JhbHNpZ24uY29tL3JlcG9zaXRvcnkvMAoGCCqGSM49BAMDA2cA\n"
"MGQCMC4lzZGQw5mpNZBmztq8huxKf9/tRUJ5yLI4q6YU+i2fjF2FRBNA64EBmljA\n"
"7dkSOwIwL9qYB0APhsLmV0LhknrzHZVvtqzg7NQaIV18BEIDZQgK3gjxYzADjHSH\n"
"5uk4mCdW\n"
"-----END CERTIFICATE-----\n";

// Function to draw the news feed
void NewsFeed::draw(DFRobot_RGBLCD1602* lcd) {
    lcd->setCursor(0, 0);
    lcd->printf("BBC NEWS");

    // Lcd scrolling functionality
    if (!headlines.empty()) {
        newsFeed_mutex.lock();
        std::string text = headlines[headlineIndex];

        // Adds spaces between the headlines
        text = "                " + text + "                ";

        std::string window = text.substr(scrollingIndex, lcdWidth);

        lcd->setCursor(0, 1);
        lcd->printf("%s", window.c_str());

        scrollingIndex++;  // Moves the scrolling index to the next

        if (scrollingIndex + lcdWidth > text.length()) { // Cheks if we are at the end of the text
            scrollingIndex = 0;  // Start the scrolling from the beginning
            headlineIndex = (headlineIndex + 1) % headlines.size(); // Move to the next headline
        }
    }
    newsFeed_mutex.unlock();
}

// Thread task to fetch news feed api
void NewsFeed::thread_task(){
    while(1){
        printf("Fetching news feed api\n");
        fflush(stdout);
        update();
        ThisThread::sleep_for(15min); //Sleeps the thread for 15min (Since the assignment did not specify, we just picked 15min)
    }
}

void NewsFeed::update() {
    Network::networkAccess.acquire(); // only 1 thread can use network at the same time to save memory

    printf("========== Fetching News ==========\n");
    TLSSocket* socket = new TLSSocket();
    std::string newsHost = "feeds.bbci.co.uk";
    std::string newsPath = "/news/world/rss.xml";
    if ( // connect to them, if it fails just return
        !Network::openSocket(socket, network) ||
        !Network::setCert(socket, SSL_CA_PEM2) ||
        !Network::connectToHost(socket, network, newsHost.c_str(), 443)
        ) return;

    std::string httpRequest = Network::createHTTPReq(newsHost, newsPath);


    size_t bytesToSend = httpRequest.length();
    nsapi_size_or_error_t sentBytes = 0;

    printf("\nHTTP GET: %s%s\n", newsHost.c_str(), newsPath.c_str());

    // Loops as long there are more data
    while (bytesToSend) {
        sentBytes = socket->send(httpRequest.c_str() + sentBytes, bytesToSend);
        if (sentBytes < 0) { // Cheks if the sent bytes are negative, if so breaks the loop
            break;
        } else {
            printf("%d bytes\n", sentBytes);
        }

        bytesToSend -= sentBytes;
    }
    
    if (sentBytes < 0) {
      printf("Failed to send HTTP request: %d\n", sentBytes);
      while (1);
    }
    
    static char buffer[BUFFER_SIZE]; // Define the buffer with a set size
    memset(buffer, 0, BUFFER_SIZE);  // Clears the buffer
    std::string xmlChunk;
    int titleCount = 0;              // Count on how many headlines
    bool insideItem = false;
    size_t itemStart;
    size_t itemEnd;

    newsFeed_mutex.lock();
    headlines.clear();   // Clear headline vector
    newsFeed_mutex.unlock();

    while (true) {
        // Receving data
        int received_bytes = socket->recv(buffer, BUFFER_SIZE);
        if (received_bytes <= 0 || titleCount >= 3) break;  // Breaks if the byte's sent is negative or 0 or we have 3 headlines

        xmlChunk.append(buffer, received_bytes);

        // If item are not found yet and the buffer is too big, keep only the last 256
        if (!insideItem && xmlChunk.size() > 512)
            xmlChunk = xmlChunk.substr(xmlChunk.size() - 256);

        // Looks start of the <item>
        size_t firstItem = xmlChunk.find("<item>");
        if (firstItem != std::string::npos) {
            insideItem = true;
            xmlChunk.erase(0, firstItem);  // Remove everything before <item>
        }

        // Loops through all complete <item> blocks
        while ((itemStart = xmlChunk.find("<item>")) != std::string::npos &&
            (itemEnd = xmlChunk.find("</item>", itemStart)) != std::string::npos) {

            std::string itemBlock = xmlChunk.substr(itemStart, itemEnd + 7 - itemStart);

            // Index of the start and the end of the item
            size_t titleStart= itemBlock.find("<title>");
            size_t titleEnd= itemBlock.find("</title>");
            if (titleStart!= std::string::npos && titleEnd!= std::string::npos) {
                std::string title = itemBlock.substr(titleStart+ 7, titleEnd- (titleStart+ 7));  // Extracting the headline

                // Remove CDATA off the xmlChunk
                const std::string cdataStart = "<![CDATA[";
                const std::string cdataEnd = "]]>";
                if (title.find(cdataStart) != std::string::npos)
                    title = title.substr(cdataStart.length(), title.size() - cdataStart.length() - cdataEnd.length());

                // Remove unwanted characters from the xmlChunk
                title.erase(std::remove_if(title.begin(), title.end(),
                            [](char c){ return c=='<' || c=='>' || c=='[' || c==']'; }),
                            title.end());


                newsFeed_mutex.lock();
                headlines.push_back(title);  // Pushes the headlines in the vector
                newsFeed_mutex.unlock();
            }
            xmlChunk.erase(0, itemEnd + 7); // Clears the std::string to save up some space

            if (titleCount >= 3) break;
        }
    }
    // Close network socket and free up its memory, avoiding memory leak
    socket->close();
    delete socket;  

    printf("======= Success =======\n");
    Network::networkAccess.release();
}

// Button check to move between views
void NewsFeed::checkButtons() {
    if (isButtonPressed(1)) {
        menu->prevView();
    } else if (isButtonPressed(2)) {
        menu->nextView();
    }
}