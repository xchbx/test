#! /bin/bash

#效果等同于直接输入 date 命令
WWW=`date`
echo $WWW

ShowDate=$(date)
echo $ShowDate

num1=10
$[$num1+20]

touch ./--abc
rm ./--abc

#查看Linux发行版本
# cat /etc/os-release