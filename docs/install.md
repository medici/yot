安装
============

Mac(64bit) & Linux(64bit)
-----

```bash
>>> ./configure
>>> cd src
>>> make
```

运行 "Hello World"
-----

```pascal
module hello_world
import Text;

func main()
	|
		const str = "hello world";
	|
	Text.WriteString(str);
	Text.WriteLn
end;
```

```bash
>>> ./yot -o hello_world.yot
>>> ./hello_world
Hello World
```


Windows
-----

正在开发中。


编译成汇编
----------

```bash
>>> ./yot -S hello_world.yot
>>> cat hello_world.s
```
```asm
	.text
	.data
L1:
	.byte	104
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.text
	.globl	Ymain
Ymain:	pushq	%rbp
	movq	%rsp,%rbp
	leaq	L1(%rip), %rdi
	movq	$0, %rsi
	call	YWriteString
	call	YWriteLn
	popq	%rbp
	ret
```
