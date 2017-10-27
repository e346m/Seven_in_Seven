defmodule Talker do
  def loop do
    receive do
      {:greet, name} -> IO.puts("Hello #{name}")
      {:praise, name} -> IO.puts("#{name}, you're amazing")
      {:celebrate, name, age} -> IO.puts("Here's to another #{age} yours, #{name}")
    end
    loop
  end
end

pid = spawn(&Talker.loop/0)
send pid, {:greet, "Eiji"}
send pid, {:praise, "Mishiro"}
send pid, {:celebrate, "Eiji Mishiro", 25}
:timer.sleep(1000)
