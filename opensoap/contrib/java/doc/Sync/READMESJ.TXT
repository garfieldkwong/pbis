OpenSOAP For Java Sample ReadMe

--------------------------------------------------------------------------------
[�ڎ�]
1.�͂��߂�
2.�K�v���W���[��
 2.1.ApacheSOAP�𗘗p����ꍇ
 2.2.sax�𗘗p����ꍇ
3.���W���[������
 3.1.ApacheSOAP�𗘗p����ꍇ
  3.1.1.���W���[���ꗗ
  3.1.2.
 3.2.sax�𗘗p����ꍇ
  3.1.1.���W���[���ꗗ
4.���̑�
 4.1.�\�z�o�b�`�t�@�C��
 4.2.���s�o�b�`�t�@�C��

--------------------------------------------------------------------------------
1.�͂��߂�
     ���̃v���W�F�N�g�����s����ɂ́AJava2(JDK1.2�ȏ�)���C���X�g�[������Ă���
     �K�v������܂��B
     �܂��A���̃v���W�F�N�g�̎��s�ɂ�SAX2.0�Ή���XML�p�[�T���K�v�ł��B
     �i���v���W�F�N�g�ł�Xerces���g�p���Ă��܂��B)
     ���̑��K�v���W���[���Ɋւ��Ắu2.�K�v���W���[���v���Q�Ƃ��A�e��������
     ���������W���[����K�X�C���X�g�[���肢�܂��B
--------------------------------------------------------------------------------
2.�K�v���W���[��
 2.1.Java2SE�̃C���X�g�[��
     http://www.java.sun.com/jse/1.3/ja/index.html
     http://www.java.sun.com/j2se/1.3/ja/install-linux-sdk.html
     ���t�@�C�����擾���C���X�g�[�����܂��B
     ���L���ϐ���ݒ肷��B
       JAVA_HOME��JavaSE�ւ�PATH���i��F/usr/java/jdk1.3.1_01�j�ǉ����܂��B
       PATH�֏�LJAVA_HOME�z��bin(��F/usr/java/jdk1.3.1_01/bin)��ǉ����܂��B
     ����:WindowsNT/2000�n��autoexec.bat�ł͂Ȃ��A�h�}�C�R���s���[�^�h�i�E�N���b
          �N�j�h�v���p�e�B�h�h���h�h���[�U���ϐ��h�ɂčs���Ă��������B

 2.2.ApacheSOAP�𗘗p����ꍇ
      Apache Xerces XML Parcer 1.2.3 http://xml.apache.org/cerces-j
      Apache SOAP 2.0(2.2)           http://xml.apache.org/soap
      JavaMail(1.2)                  http://java.sun.com/products/javamail/
      JavaBeens(1.0.1) Activeation Framework
                          = http://java.sun.com/products/javabeans/glasgow/jaf.html
      jaf(http://java.sun.com/products/beans/glasgow/jaf.html)
          CLASSPATH��xerces.jar�Asoap.jar�Amail.jar�Aactivation.jar��ǉ�

 2.2.sax�𗘗p����ꍇ
      Apache Xerces XML Parcer 1.2.3 http://xml.apache.org/cerces-j
--------------------------------------------------------------------------------
3.���W���[������
 3.1.ApacheSOAP�𗘗p����ꍇ
  3.1.1.���W���[���ꗗ
     CalcClient.java         ���C�������N���X
     CalcClientPanel.java    ���[�UGUI�ݒ�E�C�x���g�����N���X
     CalcClientRequest.java  SOAP���b�Z�[�W����M�����N���X
   3.1.1.1. CalcClient.java
     GUI�p�l����(CalcClientPanel.java�𗘗p���āj�p�ӂ��܂��B
   3.1.1.2. CalcClientPanel.java
     �{�^�������ɂ���ăC�x���g����������悤�ɐݒ�B
     �C�x���g�����ɂ����(CalcClientRequest.java�𗘗p���ājSOAP���b�Z�[�W��
     ���M���܂��B
   3.1.1.3. CalcClientRequest.java
     �T�[�oURL��GUI�p�l�����擾��GUI�p�����[�^���Element���쐬�����M���܂��B
     ApacheSOAP(SOAP.jar)�𗘗p��invoke���܂��B
     ���F���L���b�Z�[�W������Ă��������Ƃ�������̂悤�ɁAResult�Ɋւ��Ă͌^�̌��܂���
         ���Ȃ���Ԃł��Bsoap.jar�ł͎󂯎�������_��Not Found Deserializer�ƂȂ�Exception
         ����Ă��܂��܂��B�����ŁA���̃N���X�ɂ͉��L�̂悤�ȃ}�b�s���O����������܂����B
        SOAPMappingRegistry smr = new SOAPMappingRegistry ();
        StringDeserializer sd = new StringDeserializer ();
        smr.mapTypes (Constants.NS_URI_SOAP_ENC,
                      new QName ("", "Result"), null, null, sd);

  3.1.2.���b�Z�[�W��
   3.1.2.1.���N�G�X�g
     POST /calc HTTP/1.0
     Host: localhost
     Content-Type: text/xml; charset=utf-8
     Content-Length: 478
     SOAPAction: "http://services.opensoap.jp/samples/Calc/"

     <?xml version='1.0' encoding='UTF-8'?>
     <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                        xmlns:xsi="http://www.w3.org/1999/XMLSchema-instance"
                        xmlns:xsd="http://www.w3.org/1999/XMLSchema">
       <SOAP-ENV:Body>
         <ns1:Add xmlns:ns1="http://services.opensoap.jp/samples/Calc/"
                  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
           <A xsi:type="xsd:int">2</A>
           <B xsi:type="xsd:int">5</B>
         </ns1:Add>
       </SOAP-ENV:Body>
     </SOAP-ENV:Envelope>

   3.1.2.2.���X�|���X
     <?xml version="1.0" encoding="utf-8"?>
     <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
       <SOAP-ENV:Body>
         <m:AddResponse xmlns:m="http://services.opensoap.jp/samples/Calc/">
           <Result>2.000000</Result>
         </m:AddResponse>
       </SOAP-ENV:Body>
     </SOAP-ENV:Envelope>

   3.1.2.3.fault���b�Z�[�W
     HTTP/1.1 200 OK
     Content-Type: text/xml; charset="utf-8"
     Content-Length: 388

     <?xml version="1.0"?>
     <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                        SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
       <SOAP-ENV:Body>
         <SOAP-ENV:Fault>
           <faultcode>OPENSOAP:ffffffff</faultcode>
           <faultstring>OPENSOAP Service error</faultstring>
           <detail>OPENSOAP Service Internal error</detail>
         </SOAP-ENV:Fault>
       </SOAP-ENV:Body>
     </SOAP-ENV:Envelope>

 3.2.sax�𗘗p����ꍇ
  3.1.1.���W���[���ꗗ
     CalcClient.java         ���C�������N���X
     CalcClientPanel.java    ���[�UGUI�ݒ�E�C�x���g�����N���X
     CalcClientRequest.java  Calc�T�[�r�X���b�Z�[�W�����N���X
     OpenSoapConstants.java  OpenSOAP�Ƃ��Ă̒萔��`�B
     OpenSoapEnvelope.java   SOAP���b�Z�[�WEnvelope�����N���X
     OpenSoapRequest.java    ��{SOAP���b�Z�[�W�����N���X
     OpenSoapException.java  OpenSOAP�G���[������
     XMLWriter.java          XML���͐��K�������N���X
   3.1.1.1. OpenSoapConstants.java
     ���v���W�F�N�g�S�̂Ŏg�p�����萔���W�߂��N���X�ł��B
   3.1.1.2. XMLWriter.java
     �\�񕶎��i'<''&')�Ȃǂ��G�X�P�[�v��������N���X�ł��B
   3.1.1.3. OpenSoapException.java
     faultcode/faultstring���Ǘ�����N���X�ł��B
   3.1.1.4. OpenSoapEnvelope.java
     �G���x���[�v���p�[�X����N���X�ł��B
     XMLFilterImpl���p�����Ă��܂��B
   3.1.1.5. OpenSoapRequest.java
     ���N�G�X�g�p��SOAP�v���g�R�����������Ă��܂��B
     ���N�G�X�g���������AHTTP��著�M��OpenSoapEnvelope���g�p�����X�|���X��
     �f�R�[�h���Ă��܂��B
   3.1.1.6. CalcClientRequest.java
     OpenSoapRequest���p�����Ă��܂��B
   3.1.1.7. CalcClientPanel.java
     �N���C�A���g���̃��[�U�C���^�t�F�[�X���T�|�[�g���Ă���N���X�ł��B
   3.1.1.8. CalcClient.java
     �N���C�A���g���̃��C���N���X�ł��B

  3.1.2.���b�Z�[�W��
   3.1.2.1.���N�G�X�g
     POST /calc HTTP/1.1
     Content-Length: 306
     Content-Type: text/xml
     SOAPAction: ""http://services.opensoap.jp/samples/Calc/""
     User-Agent: Java1.3.0
     Host: localhost:8070
     Accept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2
     Connection: keep-alive

     <?xml version='1.0'?>
     <SOAP-ENV:Envelope xmlns:SOAP-ENV='http://schemas.xmlsoap.org/soap/envelope/'>
       <SOAP-ENV:Body>
         <ns1:Add xmlns:ns1='http://services.opensoap.jp/samples/Calc/'
                  SOAP-ENV:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/'>
           <A>1</A>
           <B>1</B>
         </ns1:Add>
       </SOAP-ENV:Body>
     </SOAP-ENV:Envelope>

   3.1.2.2.���X�|���X
     <?xml version="1.0" encoding="UTF-8"?>
     <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
       <SOAP-ENV:Body>
         <m:AddResponse xmlns:m="http://services.opensoap.jp/samples/Calc/">
           <Result>2.000000</Result>
         </m:AddResponse>
       </SOAP-ENV:Body>
     </SOAP-ENV:Envelope>

   3.1.2.3.fault���b�Z�[�W
     HTTP/1.1 200 OK
     Content-Type: text/xml
     Content-Length: 388

     <?xml version="1.0"?>
     <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                        SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
       <SOAP-ENV:Body>
         <SOAP-ENV:Fault>
           <faultcode>OPENSOAP:ffffffff</faultcode>
           <faultstring>OPENSOAP Service error</faultstring>
           <detail>OPENSOAP Service Internal error</detail>
         </SOAP-ENV:Fault>
       </SOAP-ENV:Body>
     </SOAP-ENV:Envelope>

 3.3.�N���C�A���g���[�U�C���^�t�F�[�X
     --------------------------------------------------
     | Server          |  (1)               |         |
     --------------------------------------------------
     |   |(2)|      |(3)|��|     |(4)|  (5)   (6)     |
     --------------------------------------------------
     | (7)                                            |
     --------------------------------------------------
     (1) �T�[�oURI�̓��͈�
         ���͂́A(�T�[�oURL):(�|�[�g�ԍ�)�Ƃ��܂��B
         ���Ƃ��΁A�T�[�o��URL(�����ł�IP�Ƃ��܂�)��133.50.1.90
         �T�[�r�X�|�[�g�ԍ���8764�̏ꍇ�Ahttp://133.50.1.90:8764
         �Ƃ��܂��B
     (2)(4) ���l�P�E�Q
         �v�Z�Ώۂ̐��l����͂��܂��B
     (3) �v�Z�����̑I��
         Add       �����Z
         Subtract  �����Z
         Multiply  �|���Z
         Divide    ����Z
         Fault     fault���b�Z�[�W�𔭐������܂��B
         �𐔒l�P�E�Q�ɑ΂��s���܂��B
     (5) ���s�{�^��
         �����ɂ����SOAP���b�Z�[�W�����M���ꃌ�X�|���X���A���Ă��܂��B
     (6) ���ʕ\���i�v�Z���ʁj
         �v�Z�i�T�[�r�X�j���ʂ�\�����܂��B
         ���̏ꍇ������\�����܂��B
     (7) ���ʕ\���i�������ʁj
         Checking:���N�G�X�g�����E���X�|���X������
         �󔒁F���폈����
--------------------------------------------------------------------------------
4.���̑�
 4.1.�\�z�o�b�`�t�@�C��
  4.1.1.ApacheSOAP�𗘗p����ꍇ
        (Win)ApacheSOAPBuild.bat���g�p���܂��B
        �t�@�C�����̐����ɏ]���g�p���ɍ��킹�ĕύX���Ă��������B
  4.1.2.sax�𗘗p����ꍇ
        (Win)SAXClientBuild.bat���g�p���܂��B
        �t�@�C�����̐����ɏ]���g�p���ɍ��킹�ĕύX���Ă��������B
 4.2.���s�o�b�`�t�@�C��
  4.1.1.ApacheSOAP�𗘗p����ꍇ
        (Win)ApacheSOAPExec.bat���g�p���܂��B
        (Unix)ApacheSOAPExec.sh���g�p���܂��B
  4.1.2.sax�𗘗p����ꍇ
        (Win)SaxClientExec.bat���g�p���܂��B
        (Unix)SaxClientExec.sh���g�p���܂��B
  4.1.3.ApacheSOAP�t���g���l�����j�^
        (Win)Tunnel.bat���g�p���܂��B
        (Unix)Tunnel.sh���g�p���܂��B
        ApacheSOAP���C���X�g�[������ƕt�����Ă���Tcp�g���l�����j�^���N�����܂��B
        �ʏ���s�ɂ͉��L�̂悤�ɓ��͂��܂��B
        >java org.apache.soap.util.net.TcpTunnelGui 8070 localhost 8080
        ���̋L�q���o�b�`�t�@�C�������������̂��̂ł��B
        Port��8070��8080�փg���l�����܂��̂Ŏg�p���Ȃǂւ̓K�������Ă��������B

�⑫�F�o�b�`�t�@�C�����s���Ɂu���ϐ��̂��߂̗̈悪����܂���v�ƕ\�������(DOS)
        �����͊��ϐ��̂��߂̃�����������Ȃ�����ł��B
        �ȉ��̂悤�ɂ��Ċ��ϐ��𑝂₵�Ă����܂��傤�B
        (1)DOS�����J��
        (2)ALT�ƃX�y�[�X�L�[�̓��������ɂăv���_�E�����j���[��\�����܂��B
        (3)�v���p�e�B��I�����܂��B
        (4)MS-DOS�v�����v�g�̃v���p�e�B�p�l���\��
        (5)�������^�O��I��
        (6)���ϐ��̏����T�C�Y(V)�̃v���_�E�����j���[�ő傫�Ȓl��I��
        (7)OK�{�^������������B
        (8)��xDOS������A�ēx�N�������܂��B
