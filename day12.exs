#!/usr/bin/env elixir

File.read!("inputs/day12/prod.txt")
|> String.split("\n\n")
|> Enum.at(-1)
|> String.split("\n")
|> Enum.count(fn line ->
  [x, y | i] =
    line
    |> String.split(~r{(: |x| )})
    |> Enum.map(&String.to_integer/1)

  Enum.sum(i) * 9 <= x * y
end)
|> IO.puts()
