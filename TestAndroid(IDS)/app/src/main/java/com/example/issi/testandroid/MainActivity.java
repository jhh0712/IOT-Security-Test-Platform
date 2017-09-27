package com.example.issi.testandroid;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.Socket;
import java.net.URL;


public class MainActivity extends AppCompatActivity {

    String msg, msg2;   //수신 메시지 출력용
    int count;  //줄바꿈 위한 카운트

    private TextView Hex_data;  //Hex 출력용 TextView
    //private TextView Ascii_data;    //Ascii 출력용 TextView

    String JSON_DB; //DB 를 임시 저장할 변수

    //JSON Array 분류용 String/////////////////////////
    private static final String RESULT = "result";
    private static final String AES = "aes";
    private static final String RSA = "rsa";
    private static final String RSA2 = "rsa2";
    private static final String RSA3 = "rsa3";
    ///////////////////////////////////////////////////

    JSONArray JSON_ARRAY = null;    //JSON Array 초기화

    public int[] rsa_dec = new int[16];             //RSA 복호화 데이터 저장
    public byte[][] CipherKey = new byte[4][4];    //CipherKey 저장
    public byte[][][] State = new byte[1024][4][4];         //AES 암호화 상태값 저장
    public byte[][][] aes_enc = new byte[1024][4][4];       //AES 복호화 데이터 저장


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Hex_data = (TextView)findViewById(R.id.textView4);  //TextView 초기화
        //Ascii_data = (TextView)findViewById(R.id.textView5);    //TextView 초기화

        TCPclient tcpThread = new TCPclient();
        Thread thread = new Thread(tcpThread);
        thread.start(); //클래스 생성 및 쓰레드 초기화 후 실행
    }


    public void getData(String url){    //php 로 부터 DB 값 가져오는 함수

                BufferedReader bufferedReader = null;

                try {

                    URL url2 = new URL(url);    //매개변수로부터 url 선언
                    HttpURLConnection con = (HttpURLConnection) url2.openConnection();  //HttpURLConnection 이용해 해당 URL로 연결

                    bufferedReader = new BufferedReader(new InputStreamReader(con.getInputStream()));   //데이터를 읽어오기 위해 선언

                    String result;

                    result = bufferedReader.readLine(); //데이터를 읽어옴

                    JSON_DB=result; //JSON Array 만들기 위해 String에 해당 데이터 저장

                    DataDivide();   //데이터 분류 함수 실행


                }catch(Exception e){    //예외처리
                    Log.e("HTTP", "Error", e);
                }

            }


    protected void DataDivide(){    //데이터 분류 함수

        try {
            JSONObject jsonObj = new JSONObject(JSON_DB);   //JSON Object를 만듦
            JSON_ARRAY = jsonObj.getJSONArray(RESULT);     //Object로 Array 생성

            for(int i=0;i<JSON_ARRAY.length();i++){
                JSONObject c = JSON_ARRAY.getJSONObject(i); //Array로부터 Object를 가져옴

                //JSON 데이터로부터 필요한 데이터 추출
                String aes = c.getString(AES);
                String rsa = c.getString(RSA);
                String rsa2 = c.getString(RSA2);
                String rsa3 = c.getString(RSA3);
                ///////////////////////////////////////

                RSA m_rsa = new RSA();  //RSA 클래스 사용하기 위해 생성

                //JSON 데이터로부터 추출한 데이터를 RSA 클래스의 int 형 변수로 형변환하여 저장
                m_rsa.pq = Integer.parseInt(rsa);
                m_rsa.n = Integer.parseInt(rsa2);
                m_rsa.e = Integer.parseInt(rsa3);
                ////////////////////////////////////

                String[] temp = new String[16]; //DB 데이터 분류하기 위해 선언

                char[] temp_state = {0, }; //String 형태의 RSA로 암호화된 AES키를 Char형으로 변환하기 위해 선언
                int[] i_temp = new int[16]; //RSA 복호화 하기 위해 선언

                temp_state = aes.toCharArray(); //char형으로 형변환

                int k=0;
                int count=0;

                for(int x =0; x< temp_state.length;x++)
                {
                    if(temp_state[x] != '!') {  //배열값중 !가 없으면
                        if(count ==0)   //처음값은 대입을 위해 카운트값 검사
                            temp[k] = String.valueOf(temp_state[x]);    //String에 배열값 대입
                        else
                            temp[k] += temp_state[x];   //String에 배열값 추가

                        count++;
                    }
                    else    //!가 있으면
                    {
                        if(x==0)
                            k=0;
                        else {
                            k++;    //k 증가
                            count =0;   //카운트 초기화
                        }
                    }
                }

                for(int x=0; x<16; x++)
                    i_temp[x] = Integer.parseInt(temp[x]);  //RSA 복호화에 사용하기위해 형변환

                rsa_dec = m_rsa.Decrypt(i_temp);    //RSA 복호화

                k=0;

                for(int x=0; x<4; x++)
                    for(int y=0; y<4; y++)
                        CipherKey[x][y] = (byte)rsa_dec[k++];   //복호화된값 CipherKey에 대입

            }

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }


    private class TCPclient implements Runnable {

        private static final String serverIP = "192.168.0.57";  //ip 주소
        private static final int serverPort = 7777;    //포트번호
        private Socket inetSocket = null;   //소켓 초기화
        int k =0;
        int len = 0;

        int count2 = 0;
        int count3 = 0;
        int flag =0;


        public void run() {
            try
            {
                inetSocket = new Socket(serverIP, serverPort);  //소켓 생성
                try
                {
                    InputStream in = inetSocket.getInputStream();   //바이트 단위로 데이터 읽기 위해 선언


                    while(true) {
                        byte[] data = new byte[1024];

                        in.read(data);  //바이트 배열만큼 데이터를 읽음

                        for (int i = 0; i < data.length; i++) {
                            if (data[i] == '\0') //바이트 배열의 데이터 끝에서 빠져나옴
                            {
                                len = i;
                                break;
                            }
                        }
                        flag =0;
                        count2=0;
                        count3=0;

                        count2 = len / 16;
                        count3 = len % 16;

                        if(count3 != 0)
                            count2++;

                        getData("http://192.168.0.57/TestPlatform.php");    //getData 함수 실행

                        AES m_aes = new AES();  //AES 클래스 사용하기 위해 생성

                        k = 0;


                        for(int m = 0; m < count2; m++) {
                            for (int i = 0; i < 4; i++) {
                                for (int j = 0; j < 4; j++) {
                                    State[m][i][j] = data[k++];    //상태값에 수신 데이터값 대입

                                    if (k == len) {
                                        flag = 1;
                                        break;
                                    }
                                }
                                if (flag == 1)
                                    break;
                            }
                            if(flag == 1)
                                break;
                        }

                        for(int i = 0; i < count2; i++)
                            aes_enc[i] = m_aes.Decrypt(CipherKey, State[i]);  //AES 복호화

                        byte[] enc_data = new byte[len];

                        k = 0;
                        flag =0;

                        for(int m = 0; m < count2; m++)
                            for (int i = 0; i < 4; i++)
                                for (int j = 0; j < 4; j++)
                                    enc_data[k++] = aes_enc[m][i][j];  //복호화값 대입

                        count = 1;    //카운트 초기화

                        for (int i = 0; i < len; i++) {
                            //if (enc_data[i] == '\0') //바이트 배열의 데이터 끝에서 빠져나옴
                                //break;

                            if (i == 0) {   //첫값은 대입
                                if (enc_data[i] > 0)    //unsigned를 지원하지 않으므로 값을 검사함
                                    msg = String.format("%02X\t", enc_data[i]); //msg String에 Hex 형태로 데이터 대입
                                else    // -값이 나오면 int형으로 변환하여 & 연산을 통해 unsigned로 표현할 수 있는 범위까지 나타내줌
                                    msg = String.format("%02X\t", ((int) enc_data[i] & 0xff)); //msg String에 Hex 형태로 데이터 대입
                            } else {    //이후값은 추가
                                if (enc_data[i] > 0)
                                    msg += String.format("%02X\t", enc_data[i]);
                                else
                                    msg += String.format("%02X\t", ((int) enc_data[i] & 0xff));
                            }

                            if (count % 16 == 0)
                                msg += "\r\n";  //16바이트당 줄바꿈

                            count++;
                        }

                        Hex_data.post(new Runnable() {
                            @Override
                            public void run() {
                                Hex_data.setText(msg);  //TextView에 msg String 출력
                            }
                        });
/* ASCII 데이터 출력시 사용
                        count = 1;    //카운트 초기화

                        for (int i = 0; i < data.length; i++) {
                            if (data[i] == '\0') //바이트 배열의 데이터 끝에서 빠져나옴
                                break;

                            if (i == 0) {   //첫값은 대입
                                if (data[i] > 0)    //unsigned를 지원하지 않으므로 값을 검사함
                                    msg2 = String.format("%d\t", data[i]);  //msg2 String에 Ascii 형태로 데이터 대입
                                else    // -값이 나오면 int형으로 변환하여 & 연산을 통해 unsigned로 표현할 수 있는 범위까지 나타내줌
                                    msg2 = String.format("%d\t", ((int) data[i] & 0xff));   //msg2 String에 Ascii 형태로 데이터 대입
                            } else {    //이후값은 추가
                                if (data[i] > 0)
                                    msg2 += String.format("%d\t", data[i]); //msg2 String에 Ascii 데이터 추가
                                else
                                    msg2 += String.format("%d\t", ((int) data[i] & 0xff));  //msg2 String에 Ascii 데이터 추가
                            }

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
*/
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