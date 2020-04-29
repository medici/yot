过程(procedure)
============

定义
-----

过程有两种类型：正规过程(proper procedure)和函数过程(function procedure)。

过程声明包括头部(procedure heading)与主体(procedure body)。头部指定过程名称，参数以及返回值类型。主体包含声明和语句。

```pascal
	ProcedureDeclaration = ProcedureHeading ProcedureBody.
	ProcedureHeading = func identdef [formalParameters].
	ProcedureBody = DeclarationSequence[StatementSequence]
									[return expression] end.
	DeclarationSequence = [const {constDeclaration ";"}]
												[type {typeDecaration ";"}]
												[var {varDeclaration ";"}]
												[{ProcedureDeclaration ";"}].
```


function vs procedure
---------------------

 在 Pascal-like 语言中，function 与 procedure 是不同的实体(entry)；简单地说，就是有没有返回值的区别。procedure 是一系列语句的集合。在行为与语法上也有不同：procedure 调用是语句，你不能在表达式中使用；function 不能单独使用，而是作为其它语句的组成部分。因为，Pascal-bred 程序员能很好地区别这两者。

在 C-like 语言和众多当代语言当中，这两者已经没有区别了；在静态类型语言当中，procedure 只是有特殊返回类型的function。这能说明为什么它们是可互换的。

在函数式语言中，根本就没有 procedure -- 一切都是function。

Yot 是 Pascal-like 语言，并且遵守以下命名规则：

|  实体   | 命名  |    示例 |  
|  ----  | ----  |
| 常量、变量  | 以小写字母开头；名词或形容词 | version, wordSize, full |
| Types |  以大写字母开头；名词 | File, TextFrame |
| Procedures  | 以大写字母开头；动词 |  WriteString  |
| Functions   | 以大写字母开头；名词或形容词 | Position, Empty, Equal |
| Modules     | 以大写字母开头；名词 |   Files, TextFrames |


值参数与引用参数
-------------

当参数以关键字 `var` 进行声明时，为引用参数；否则为值参数。比如：

```pascal
module example
import Text;

func Start
	|
		var a, b, c, r1, r2, i1, i2 :real

		func ComputeRoots(a, b, c:real, var r1, r2, i1, i2:real) : real
			r1 := r2 + i1 + i2;
			a := a + b + c;

			return a
		end;

	|

	a := 1.1;
	b := 2.2;
	c := 3.3;

	r1 := 1.1;
	r2 := 2.2;
	r3 := 3.3;
	r4 := 4.4;

	Text.WriteReal(ComputeRoots(a, b, c, r1, r2, r3, r4));  (* => 6.6 *)
	Text.WriteLn;
	Text.WriteReal(r1); (* => 9.9 *)
	Text.WriteLn;
	Text.WriteReal(a); (* => 1.1 *)
	Text.WriteLn;
end;

end.
```

a、b、c都为值参数，r1､ r2､ i1､ i2为引用参数。当调用 ComputeRoorts 函数后，a的值不会变，而r1的值变了。