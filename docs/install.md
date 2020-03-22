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