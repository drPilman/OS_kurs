sudo swapoff /swap.img
sudo swapon /swap.img
docker run -it --init --net=host drpilman/server2