
数值类型
============

整型
-----

```pascal
module number
	import Text;

	func main()
		|  var i, j:integer;  |

		i := 13;
		j := 5 * 4;

		i := i mod 12; (* 取模 *)
		j := j div 2;
		Text.WriteInt(i);  (*  => 1  *)
		Text.WriteLn;
		Text.WriteInt(j);  (*  => 10 *)
		Text.WriteLn;
	end;
end.
```

浮点型
-----

```pascal
module number
	import Text;

	func main()
		|  var i, j:real;  |

		i := 13.2;
		j := 5.3e12;

		j := j / 2;         (* 注意：浮点数除号是 `/`, 整型是 `div` *)
		Text.WriteReal(i); 
		Text.WriteLn;
		Text.WriteReal(j); 
		Text.WriteLn;
	end;
end.
```