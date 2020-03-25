
布尔类型
============

布尔类型只有两个值：`true` 和 `false`：

```pascal
module boolean
	func main()
		|
			var t, f : boolean;
		|

		t := true;
		f := false;
	end;
end.
```

也可以通过布尔表达式进行赋值：

``` pascal
	t := 1 > 2;
	f := 2 # 3;
```

操作符
-----

布尔操作符有`and`、`or`和`~`（取反）：

```pascal
	t := ~false;
	f := (1 > 2) or (2 > 3);
```
