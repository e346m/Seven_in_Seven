import java.util.Random;
class Philosopher extends Thread {
  private Chopstick first, second;
  private Random random;
  private int thinkCount;

  public static void main(String[] args) throws InterruptedException {
    Philosopher[] philosophers = new Philosopher[5];
    Chopstick[] chopsticks = new Chopstick[5];

    for (int i = 0; i < 5; ++i)
      chopsticks[i] = new Chopstick(i);
    for (int i = 0; i < 5; ++i) {
      philosophers[i] = new Philosopher(chopsticks[i], chopsticks[(i + 1) % 5]);
      philosophers[i].start();
    }
    for (int i = 0; i < 5; ++i)
      philosophers[i].join();
  }

  public Philosopher(Chopstick left, Chopstick right){
    if(left.getId() < right.getId()) {
      first = left; second = right;
    } else {
      first = right; second = left;
    }
    random = new Random();
  }
  public void run() {
    try {
      while(true) {
        ++thinkCount;
        if(thinkCount % 10 == 0)
          System.out.println("Philosopher " + this + " has thought " + thinkCount + " times");
        Thread.sleep(random.nextInt(1000));     // THink for a while
        synchronized(first) {                    // Grap left chopstick
          synchronized(second) {                 // Grap right chopstick
            Thread.sleep(random.nextInt(1000)); // Eat for a while
          }
        }
      }
    } catch(InterruptedException e) {}
  }
}

class Chopstick {
	private int id;
	public Chopstick(int id) { this.id = id; }
  public int getId() { return id; }
}
