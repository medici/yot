
记录类型
============

记录的声明格式：

```pascal
	record
		[字段名 : 类型]
	end;
```

例子：

```pascal
module records
	import Text;

	type 
		person = record
			age: integer;
			name: array 34 of char;
		end;
	var s : person;

	func main()
		s.age := 10;
		Text.WriteInt(s.age);
		Text.WriteLn;
		s.name := "str";
		Text.WriteString(s.name);
		Text.WriteLn;

	end;
end.

```