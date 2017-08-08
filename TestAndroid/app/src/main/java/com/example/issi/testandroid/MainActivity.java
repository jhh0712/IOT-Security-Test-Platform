package com.example.issi.testandroid;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.io.InputStream;
import java.net.Socket;


public class MainActivity extends AppCompatActivity {

    String msg, msg2;   //수신 메시지 출력용
    int count;  //줄바꿈 위한 카운트

    private TextView Hex_data;  //Hex 출력용 TextView
    private TextView Ascii_data;    //Ascii 출력용 TextView

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Hex_data = (TextView)findViewById(R.id.textView4);  //TextView 초기화
        Ascii_data = (TextView)findViewById(R.id.textView5);    //TextView 초기화

        TCPclient tcpThread = new TCPclient();
        Thread thread = new Thread(tcpThread);
        thread.start(); //클래스 생성 및 쓰레드 초기화 후 실행
    }

    private class TCPclient implements Runnable {

        private static final String serverIP = "192.168.0.57";  //ip 주소
        private static final int serverPort = 7777;    //포트번호
        private Socket inetSocket = null;   //소켓 초기화


        public void run() {
            try
            {
                //Log.d("TCP", "Connecting...");
                inetSocket = new Socket(serverIP, serverPort);  //소켓 생성
                try
                {
                    InputStream in = inetSocket.getInputStream();   //바이트 단위로 데이터 읽기 위해 선언

                    while(true) {

                        byte[] data = new byte[1024];

                        in.read(data);  //바이트 배열만큼 데이터를 읽음

                        count=1;    //카운트 초기화

                        for (int i = 0; i < data.length; i++) {
                            if(data[i] == '\0') //바이트 배열의 데이터 끝에서 빠져나옴
                                break;

                            if(i==0)
                                msg = String.format("%02X\t\t", data[i]);   //msg String에 Hex 형태로 데이터 대입
                            else
                                msg += String.format("%02X\t\t", data[i]);  //msg String에 Hex 데이터 추가

                            if (count % 8 == 0)
                                msg += "\r\n";  //8바이트당 줄바꿈

                            count++;
                        }

                        Hex_data.post(new Runnable() {
                            @Override
                            public void run() {
                                Hex_data.setText(msg);  //TextView에 msg String 출력
                            }
                        });

                        count=1;    //카운트 초기화

                        for (int i = 0; i < data.length; i++) {
                            if(data[i] == '\0') //바이트 배열의 데이터 끝에서 빠져나옴
                                break;

                            if(i==0)
                                msg2 = String.format("%d\t\t", data[i]);    //msg2 String에 Ascii 형태로 데이터 대입
                            else
                                msg2 += String.format("%d\t\t", data[i]);   //msg2 String에 Ascii 데이터 추가

                            if (count % 8 == 0)
                                msg2 += "\r\n"; //8바이트당 줄바꿈

                            count++;
                        }

                        Ascii_data.post(new Runnable() {
                            @Override
                            public void run() {
                                Ascii_data.setText(msg2);   //TextView에 msg2 String 출력
                            }
                        });
                    }

                }
                catch (Exception e) //예외 상황 처리
                {
                    Log.e("TCP", "C: Error1", e);
                }
                finally //소켓 해제
                {
                    inetSocket.close();
                }
            }
            catch (Exception e) //예외 상황 처리
            {
                Log.e("TCP", "C: Error2", e);
            }
        }
    }
}