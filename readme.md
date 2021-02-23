## 1.STL格式简介

STL只能用来表示封闭的面或者体。stl文件有两种：一种是ASCII明码格式，另一种是二进制格式。

## 2.二进码 STL

由于文字STL档会非常大，因此有二进码版的STL。二进码STL档有个80[字符](https://baike.baidu.com/item/字符)的[标头](https://baike.baidu.com/item/标头)（内容通常都被忽略但绝不可用"solid"来开始！这样会使不少软件认为这是文字STL档）。接着是一个4[字节](https://baike.baidu.com/item/字节)无号整数，它表示这文件中的[三角形](https://baike.baidu.com/item/三角形)数。再来是描述每个三角形的信息。之后就结束。

每个三角形以12个32位浮点数来描述：3个描述法线，接着三顶点的X/Y/Z座标─正如文字STL一样。之后是一个2[字节](https://baike.baidu.com/item/字节)无号整数(unsigned short)，它代表“属性字符数”─在标准格式中它该是0，因为多数软件不明白其它事项。

虽然文件内没有说明，浮点数以[little-endian](https://baike.baidu.com/item/little-endian)的[IEEE 754](https://baike.baidu.com/item/IEEE 754)表示。

```
UINT8[80] – HeaderUINT32 – Number of trianglesforeach triangleREAL32[3] – Normal vectorREAL32[3] – Vertex 1REAL32[3] – Vertex 2REAL32[3] – Vertex 3UINT16 – Attribute byte countend
```

## 3.二进码STL的颜色

二进码STL至少有两种非标准方式来加上颜色信息：

- VisCAM和SolidView软件用每个三角形后的2个“属性字符数”存放15比特[RGB](https://baike.baidu.com/item/RGB)色码：

- - 比特 0到 4是蓝的强度(0~31)。
  - 比特 5到 9是绿的强度(0~31)。
  - 比特10到14是红的强度(0~31)。
  - 比特15是1表颜色有效；0表无效（同正常的STL档）。

- Materialise Magics软件用开头的80位标头表示模型整体颜色。若有使用颜色，标头某处会是文字字符串"COLOR="接着4[字节](https://baike.baidu.com/item/字节)的[RGB](https://baike.baidu.com/item/RGB)A(透明度)，[值域](https://baike.baidu.com/item/值域)为0–255。这是整个模型的颜色，每个小面可以另外指定。这软件还可以指定更详细的表面特性，即物质(计算机图形)。在"COLOR=RGBA"后可加上ASCII字符串",MATERIAL="，接着3颜色(3×4位)：

先是[漫反射](https://baike.baidu.com/item/漫反射)的颜色，第二个是反射高光颜色，第三是[环境光](https://baike.baidu.com/item/环境光)。物质设置优于颜色。每个小面的颜色是用三角形后的2个“属性字符数”以下列方式来存放：

- 

- - 比特 0到 4是红的强度(0~31)。
  - 比特 5到 9是绿的强度(0~31)。
  - 比特10到14是蓝的强度(0~31)。
  - 比特15是0表用小面颜色；1表用模型颜色。

上述两种小面颜色信息中，2比特里的红/绿/蓝顺序相反，因此，虽然这些格式容易兼容，反序表示并非如此！更糟的是：通用的STL读取器不能自动分别它们！也无法指定小面有透明度！因为没有α值！虽然对当前的快速原型机而言，这不重要。

## 4.STL的文件格式

### ASCII格式

ASCII码格式的STL文件逐行给出三角面片的几何信息，每一行以1个或2个关键字开头。

在STL文件中的三角面片的信息单元 facet 是一个带矢量方向的三角面片，STL三维模型就是由一系列这样的三角面片构成。

整个STL文件的首行给出了文件路径及文件名。

在一个 STL文件中，每一个facet由7 行数据组成，

facet normal 是三角面片指向实体外部的法矢量坐标，

outer loop 说明随后的3行数据分别是三角面片的3个顶点坐标，3顶点沿指向实体外部的法矢量方向逆时针排列。 [2] 

ASCII格式的STL 文件结构如下：

```
明码://字符段意义
solidfilenamestl//文件路径及文件名
facetnormalxyz//三角面片法向量的3个分量值
outerloop
vertexxyz//三角面片第一个顶点坐标
vertexxyz//三角面片第二个顶点坐标
vertexxyz//三角面片第三个顶点坐标
endloop
endfacet//完成一个三角面片定义
 
......//其他facet
 
endsolidfilenamestl//整个STL文件定义结束
```

### 二进制格式

二进制STL文件用固定的字节数来给出三角面片的几何信息。

文件起始的80个字节是文件头，用于存贮文件名；

紧接着用 4 个字节的整数来描述模型的三角面片个数，

后面逐个给出每个三角面片的几何信息。每个三角面片占用固定的50个字节，依次是:

3个4字节浮点数(角面片的法矢量)

3个4字节浮点数(1个顶点的坐标)

3个4字节浮点数(2个顶点的坐标)

3个4字节浮点数(3个顶点的坐标)个

三角面片的最后2个字节用来描述三角面片的属性信息。

一个完整二进制STL文件的大小为三角形面片数乘以 50再加上84个字节。

二进制:

```
UINT8//Header//文件头
UINT32//Numberoftriangles//三角面片数量
//foreachtriangle（每个三角面片中）
REAL32[3]//Normalvector//法线矢量
REAL32[3]//Vertex1//顶点1坐标
REAL32[3]//Vertex2//顶点2坐标
REAL32[3]//Vertex3//顶点3坐标
UINT16//Attributebytecountend//文件属性统计
```

## 5.发现的问题

1.面的法向量->顶点法向量.如何转化？？是否需要处理，具体怎样做似乎不需要处理

2.2个颜色信息怎么办

3.STLloader::readSTL_ASCII 优化读取速度；

4.将顶点柔和一下，用曲线拟合表面

5.加入阴影



求出的顶点法线的值并不重要,我们只需要在乎他的**方向(正和反),**如上图N1是正方向,求出它的反方向就应该是

```
N1 = (P0 - P1) 叉乘  (P1 - P2）；  是反
```



**第二种计算顶点法向量的方式:** 如果一个顶点是三个平面的共同顶点,那么通过三个面法线计算出顶点法线

```
Np = (N0 +N1 + N2)  / 3
```