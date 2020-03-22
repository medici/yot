
入门
============

Module
-----

模块必须以常量、类型、变量、函数的顺序进行声明，否则编译器会报错：

```pascal
module hello_world
  const str = "hello world"; (* 这里是常量声明 *)

  var i, j : integer;  (* 这里是变量声明 *)

  func WriteString(s: array of char) (* 这里是函数声明 *)
    |
      var i: integer;
    |
    i := 0;
    while s[i] # 0x do 
      Write(s[i]);
      i := i + 1
    end;
  end;

  func main() 
    WriteString(str);
  end;
end.
```

模块名称必须与文件名称一致；模块以`end`与`.`结束。

Function
-------------

函数可以包含函数。所有的常量、变量、函数声明必须放在 `|` `|` 之间；并且按照常量、变量、类型、函数的顺序进行声明。

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
end.

```


数据类型
-------------
yotlang 支持的数据类型有integer, boolean, real, char ,array, record。



```pascal

module datatype

  const int = 18;

  type 
    person = record              (* record *)
      name : array 32 of char;   (* array  *)
      age  : integer;
    end;

  var i : integer;
      r : real;
      c : char;
      obj : person;


  func main()
    i := int;
    r := 16.66;
    c := 0bx;
    obj.name = "harry potter";
    obj.age = 6;
  end;

end.

```

常量用 `=` 进行赋值；变量则用 `:=` 。 字符类型是以 `x` 结束的16进制；如果值是以'a', 'b', 'c', 'd', 'e','f'中的一个开头，必须在前面加 `0`，以便跟变量名区分。

通用操作符有：`=`(等于)、`#`(不等于)、`>`、 `>=`、 `<`、 `<=`。
布尔操作符有：`or`、 `and`、 `~`。