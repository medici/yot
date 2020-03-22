语句
============


if
-----

if 语句可以按照以下格式进行使用:
```pascal
	if condition then
		statementSequence
	elsif condition then
		statementSequence
	else
		statementSequence
	end;
```

例子：

```pascal
module ifstatement
	import Text;

    const
        tru* = true;
        fls = false;
        

    func main ()
        |
            var 
                r, rr : real;
        |

        
        r := 1.2;
        rr := 3.6;

        
        if r = rr then
            r := 2.8
        end;


        if r < rr then
            r := 3.8
        elsif r = rr then
            r := 3.8
        else
            r := 4.8
        end;
        Text.WriteReal(r);
        Text.WriteLn();
    end;

end.
```


for
-----
for 循环格式如下：

```pascal
	for ident := expresson to expresion [by constExpression] do
		statementSequence
	end;
```

例子：

```pascal
module forstatement
	import Text;
	
	func main() 
		|
			var i, j,z : integer;
		|

		i := 10;
		j := 0;
		
		for z := 1 to i by 2 do
			j := j + 1
		end;

		Text.WriteInt(j);
		Text.WriteLn;
		Text.WriteInt(i);
		Text.WriteLn;

		i := 10;
		j := 0;
		
		for z := 1 to i  do
			j := j + 1
		end;
	end;
end.
```


repeat 
-----

repeat 语句会重复执行指定的语句，直到条件语句为 `true`。

```pascal
	repeat
		statementSequence
	util expression;
```

statementSequence 至少会执行一次。例子：

```pascal
module repeatstatement
	import Text;

	func main ()
		|
			var i, j : integer;
		|

		i := 0;
		j := -99;

		repeat 
			j := j + 2
		until j >= i + 2;

		Text.WriteInt(j);
		Text.WriteLn;
	end;
end.
```



while 
-----

while 语句， 只要任何的布尔表达式为`true`，相应的语句串便会执行。

```pascal
	while expression do
		statementSequence
		{ elsif expression do statementSequence}
	end
```

例子：

```pascal
module whilestatement
	import Text;

	func main() 
		|
			var i, j : integer;
		|

		i := 10;
		j := 0;

		while i > j do
			j := j + 1
		end;
		
		i := 10;
		j := 0;
		while i > j do
			j := j + 1
		elsif j < 20 do
			j := j + 1
		elsif i > 1 do
			i := i - 1
		end;

		Text.WriteInt(j);
		Text.WriteLn;
		Text.WriteInt(i);
		Text.WriteLn
	end;
end.
```