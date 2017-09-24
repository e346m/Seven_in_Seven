class Philosopher extends Thread {
  private Chopstick left, right;
  private Random rondom;

  public Philosopher(Chopstick left, Chopstick right){
    this.left = left; this.right = right;
    random = new Random();
  }
  public void run(){
    try {
      while(true) {
        Thread.sleep(random.nextInt(1000));     // THink for a while
        synchronized(left) {                    // Grap left chopstick
          synchronized(right) {                 // Grap right chopstick
            Thread.sleep(random.nextInt(1000)); // Eat for a while
          }
        }
      }
    } catch(InterruptedException e) {}
  }
}
