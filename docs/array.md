
数组
============

数组的声明格式：

```pascal
	array 数组长度 of 类型
```

例子：

```pascal
module arr
	import Text;

	func main()
		|
			var int : array 100 of integer;
				i : integer;
		|
		i := 10;
		int[i] := 99;
		Text.WriteInt(int[i]);
		WriteLn
	end;
end.
```
