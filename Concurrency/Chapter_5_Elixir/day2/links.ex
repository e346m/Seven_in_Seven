defmodule LinkTest do
  def loop_system do
    Process.flag(:trap_exit, true)
    loop
  end
  def loop do
    receive do
      {:exit_because, reason} -> exit(reason)
      {:link_to, pid} -> Process.link(pid)
      {:EXIT, pid, reson} -> IO.puts("#{inspect(pid)} exited becase #{reson}")
    end
    loop
  end
end
