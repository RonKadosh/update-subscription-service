package bgu.spl.net.impl.stomp;


import bgu.spl.net.srv.Server;
import java.util.Objects;

public class StompServer {

    public static void main(String[] args) {    
        //TODO: implement this
        if (args.length!=2){
            System.out.println("args should be : <port> <servertype(tpc/reactor)>");
        }
        else{
            int port=0;
            try{
            port=Integer.parseInt(args[0]);}
            catch (NumberFormatException e){ System.out.println("wront port input(number only)"); };
            if (Objects.equals(args[1], "tpc"))
                Server.threadPerClient(
                        port, //port
                        StompProtocol::new, //protocol factory
                        StompMessageEncoderDecoder::new //message encoder decoder factory
                ).serve();
            else if (Objects.equals(args[1], "reactor"))
                Server.reactor(10,port,StompProtocol::new,StompMessageEncoderDecoder::new).serve();
            else {
                System.out.println("wrong server type");
            }
        }


    }
}
