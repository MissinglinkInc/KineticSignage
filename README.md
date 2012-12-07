KineticSignage
==============

## KinectServer
Kinectのサーバ。Skeletal ViewerサンプルにWinSockを組み込んだもの。あまり非同期とか気にしたくないので、Skeltal検知メソッドのなかでUDPをぶっ放している。
2.5GHz程度のC2Dで30%つかうのでi5くらいがいい。メモリは300MBくらい食うので最悪4GBくらい積んでないとやばい。
UDPポート65487番
骨格情報をUDPで同一ネットワーク内にブロードキャストする。検知人数が一人の場合、たぶん100kbpsくらい。
UDPは検知ループごとにパケットを送る。UDPなので欠落がある可能性はあるが、時系列で情報の欠落があるが、そのほかはない。たとえば人間情報の欠落、骨格の一部情報の欠落などは起こらない。

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
サーバから送られるパケットを見て描画するデジタルサイネージクライアント。あまりメモリは不要。
同期的（blocking）な受信部分と描画部でスレッドを分けている。グローバル変数で角度値を共有。（read/writeが明確に分かれているので、queueやmutexをつかう必要がない。たぶん）
描画はDirect2D。ハードコードされたディレクトリの写真をすべて読みに行く。
右手が右ひじより上にあるもしくは左手が左ひじより上にある場合、
角度=arctan(手首y-ひじy,手首x-ひじx)
（右手優先）

角度をもとに、写真を切り替えていく。