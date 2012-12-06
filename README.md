KineticSignage
==============

## KinectServer
Kinectのサーバ。Skeletal Viewerサンプルにちょっとした計算とWinSockを組み込んだもの。あまり非同期とか気にしたくないので、Skeltal検知メソッドのなかでUDPをぶっ放している。
2.5GHz程度のC2Dで30%つかうのでi5くらいがいい。メモリは300MBくらい食うので最悪4GBくらい積んでないとやばい。
UDPポート65487番（65535-AKB48という意味ｗ）

右手が右ひじより上にあるもしくは左手が左ひじより上にある場合、
角度=arctan(手首y-ひじy,手首x-ひじx)
（右手優先）

計算した後、角度をUDPで同一ネットワーク内にブロードキャストする。
これにより、一台のKinectから発せられるデータを複数機器で共有可能。

## KineticSignage
サーバから送られるパケットを見て描画するデジタルサイネージクライアント。あまりメモリは不要。
同期的（blocking）な受信部分と描画部でスレッドを分けている。グローバル変数で角度値を共有。（read/writeが明確に分かれているので、queueやmutexをつかう必要がない。たぶん）

描画はDirect2D。ハードコードされたディレクトリの写真をすべて読みに行く。
KinectServerから発せられる角度情報をもとに、写真を切り替えていく。