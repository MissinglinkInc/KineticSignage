KineticSignage
==============

## KinectServer
### 概要
Kinectのサーバ。Skeletal ViewerサンプルにWinSockを組み込んだもの。あまり非同期とか気にしたくないので、Skeltal検知メソッドのなかでUDPをぶっ放している。

### requirements
2.5GHz程度のC2Dで30%つかうのでi5くらいがいい。メモリは300MBくらい食うので最悪4GBくらい積んでないとやばい。
Kinect SDK、Direct2DはWindows 7以上を要求すると思われる。

### 仕様と注意
骨格情報をUDPポート65487番で同一ネットワーク内にブロードキャストする。検知人数が一人の場合、たぶん100kbpsくらい。

UDPは検知ループごとにパケットを送る。UDPなので欠落がある可能性はある。その場合、時系列（ある一瞬）で情報の欠落があるが、そのほかはない。たとえば人間情報の欠落、骨格の一部情報の欠落などは起こらない。（その場合は、UDPヘッダのチェックサムによりはじかれると思われる）

UDPはTCPのようにシーケンス番号やACKがないため、クライアント側では、時系列でのパケット欠損、時系列の前後に気づくことはできない。そのため、当プログラムは小規模かつ安定的なローカルネットワークにおける運用を想定している。
実装されているコードではブロードキャストするため、ルータによってセグメントが分割されたネットワークでの運用を推奨する。（多くの場合、ルータはUDPブロードキャストパケットを破棄する）

### プロトコルは以下：
    [
      [
        [float x, float y, float z, float w]...
      ]...
    ]...
#### skeleton position
x,y,z,wはfloatの座標データである。この固定長の配列は骨格座標である。

cf. struct Vector4, NuiSensor.h, Kinect SDK

#### skeleton positions = human
骨格座標は複数含められる。これはNuiSensor.hで列挙されているNUI_SKELETON_POSITION_*のインデックスと同じインデックスが振られている

cf. enum NUI_SKELETON_POSITION_INDEX, NuiSensor.h, Kinect SDK

#### humans
skeleton positions (つまりhuman)の配列としてのhumans

cf. struct NUI_SKELETON_FRAME.SkeletonData

この配列コンテナはMessagePackのシリアライザによってシリアライズされ、ひとつのUDPデータグラムしてブロードキャストされる。
UDPペイロードはこの一種類だけである。ペイロードをMessagePackでデシリアライズすれば、どのような環境でも上記配列が展開される。
これにより、一台のKinectから発せられるデータを複数機器で共有可能。

## KineticSignage
### 概要
サーバから送られるパケットを見て描画するデジタルサイネージクライアント。あまりメモリは不要。Direct2DなのでWindows 7が必須。
同期的（blocking）な受信部分と描画部でスレッドを分けている。グローバル変数で角度値を共有。（read/writeが明確に分かれているので、queueやmutexをつかう必要がない。たぶん）
描画はDirect2D。ハードコードされたディレクトリの写真をすべて読みに行く。

右手が右ひじより上にあるもしくは左手が左ひじより上にある場合、
角度=arctan(手首y-ひじy,手首x-ひじx)
（右手優先）

角度をもとに、写真を切り替えていく。

### ひとこと
最初はGPUメモリを使い切るバグがあった。いまは反芻して反省したので、このバグは発生しない。原因はリソースの解放がなされていないことだった。

ほかにも問題があるかもしれない。

# 開発環境
Visual C++
Windows 8 Pro, Visual Studio 2012 Ultimate
検証は、Windows 8 Pro 64bit , Core i5 2520M, NVIDIA NVS 4200M + Intel HD Graphics 3000のマシンとWindows 7 Professional SP1 64bit, Core 2 Duo P8700 , NVIDIA GeForce 9400Mの環境で行った（両マシンとも8GB RAM搭載）