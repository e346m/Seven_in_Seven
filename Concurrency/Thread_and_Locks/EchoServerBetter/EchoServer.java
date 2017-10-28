import java.io.*;
import java.net.Socket;
import java.util.concurrent.*;
import java.net.ServerSocket;
public class EchoServer {
  public static void main(String[] args) throws IOException {
    class ConnectionHandler implements Runnable {
      InputStream in; OutputStream out;
      ConnectionHandler(Socket socket) throws IOException {
        in = socket.getInputStream();
        out = socket.getOutputStream();
      }
      public void run() {
        try {
          int n;
          byte[] buffer = new byte[1024];
          while((n = in.read(buffer)) != -1) {
            out.write(buffer, 0, n);
            out.flush();
          }
        } catch (IOException e) {}
      }
    }
    int threadPoolSize = Runtime.getRuntime().availableProcessors() * 2;
    ExecutorService executor = Executors.newFixedThreadPool(threadPoolSize);
    ServerSocket server = new ServerSocket(4567);
    while(true) {
      Socket socket = server.accept();
      executor.execute(new ConnectionHandler(socket));
    }
  }
}