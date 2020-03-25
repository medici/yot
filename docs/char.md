
字符类型
============

表示
-----

字符类型是[ASCII 码](https://www.asciitable.com/)的16进制表示，以字符`x`结束；当值以字母开头时，前面应加数字`0`，以例跟变量名区分：

```pascal
	const 
		ln = 0ax; (* 换行符 *)
		A  = 41x; (* 大写字母 A *)
```

或者用长度为1的字符串表示：

```pascal
	var ch : char;
	ch := "c";
```

转换
-----

`Ord`函数可把字符转为ASCII 码的10进制表示：

```pascal
	const 
		A  = 41x; (* 大写字母 A *)
	var i :integer;

	i := Ord(A); (* i 等于 65 *)

```

打印
-----

`Write`函数可直接打印字符：

```pascal
module char;
	const 
		ln = 0ax; (* 换行符 *)
		A  = 41x; (* 大写字母 A *)
		B  = 42x; (* 大写字母 B *)
		C  = 43x; (* 大写字母 C *)

	func main()
		Write(A);
		Write(ln);
		Write(B);
		Write(ln);
		Write(C);
		Write(ln);
	end;

end.
```

执行这段代码，结果如下：

```shell
	A
	B
	C
```

比较
-----

字符类型的比较操作符有`=`，`#`，`>=`， `>`，`<=`，`<`；本质是对比字符的10进制大小：
```pascal
module char;
	func main()
	|
		var 
			bool : boolean;
			c1, c2: char;
	|
		c1 := "a";
		c2 := "b";
		bool := c1 > c2; (* false *)
		bool := c1 # c2; (* true *)
		bool := c1 = "a"; (* true *)
	end;
end.
```