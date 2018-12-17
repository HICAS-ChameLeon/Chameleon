var app = angular.module('myApp', []);

// app.controller('siteCtrl', function($scope, $http) {
//     $http.get("http://172.20.110.228/master/runtime-resources")
//         .then(function (response) {$scope.names = response.data.quantity;});
// });

app.controller('hardware', function($scope, $http) {
    $http({
        method: 'GET',
        url: 'http://172.20.110.228:6060/master/hardware-resources'
    }).then(function successCallback(response) {
        $scope.hardware = response.data.content;
        $scope.quantities = response.data.quantity;
    }, function errorCallback(response) {
        // 请求失败执行代码
    });
});

var runtimeApp = angular.module('runtimeApp', []);
runtimeApp.controller('runtimeCtrl', function($scope, $http) {
    $http({
        method: 'GET',
        url: 'http://172.20.110.228:6060/master/runtime-resources'
    }).then(function successCallback(response) {
        $scope.runtime = response.data.content;
        console.log(response.data.content);
        $scope.quantities = response.data.quantity;
        console.log(response.data.quantity);

        var DIR = '../icon/refresh-cl/';
        var vertexes = new Array();

        var my_master = {};
        vertexes[0] = my_master;
        my_master.id = 0;
        my_master.label = "master";
        my_master.shape = 'image';
        my_master.image = DIR + 'Network-Pipe-icon.png';
        my_master.title = '主节点';

        //my_slaves[0] = my_master;
        var index_slave = 1;
        var index_edge = -1;
        var index_subslave =777;
        var my_edges = [];
        var cur_index=0;

        if($scope.quantities>=2){
            my_edges = [];
            for(var i in $scope.runtime){
                var slave = $scope.runtime[i];

                var temp_slave = {}; // 添加一个顶点
                cur_index++; // 全局id
                temp_slave.label = "slave"+cur_index;
                console.log('57 '+cur_index);
                temp_slave.id = cur_index;
                temp_slave.shape ='image';
                temp_slave.image = DIR + 'Hardware-My-Computer-3-icon.png';
                temp_slave.title = JSON.stringify(slave);
                vertexes[cur_index]= temp_slave; // cur_index 同时代表 顶点集合 my_slaves 的下标
                console.info(vertexes);
                var temp_edge = {}; // 添加一条边 master -> temp_slave
                temp_edge.from = 0;
                temp_edge.to = temp_slave.id;
                temp_edge.arrows = 'to';
                index_edge++; // 边集合 my_edges 的下标
                my_edges[index_edge] = temp_edge;

                // 添加cpu节点
                var temp_cpu = {};
                cur_index++;
                temp_cpu.label = "cpu"+cur_index;
                temp_cpu.id = cur_index;
                temp_cpu.group = 'server';
                // temp_cpu.shape='image';
                // temp_cpu.image = DIR + 'Hardware-My-Computer-3-icon.png';
                vertexes[cur_index] = temp_cpu;
                // 添加连接cpu节点的边, temp_slave -> temp_cpu
                var edge_cpu = {};
                index_edge++;
                edge_cpu.from= temp_slave.id;
                edge_cpu.to = temp_cpu.id;
                edge_cpu.arrows = 'to';
                my_edges[index_edge] = edge_cpu;



                // var temp_mem

                index_slave++;

            }
        }else{
            my_edges=[];
        }


        var nodes = new vis.DataSet(vertexes);


        var edges = new vis.DataSet(my_edges);

        var container = document.getElementById('mynetwork');
        var data = {
            nodes: nodes,
            edges: edges
        };

        var options = {
            interaction: {
                navigationButtons: true,
                keyboard: true
            },
            groups: {
                'switch': {
                    shape: 'triangle',
                    color: '#FF9900' // orange
                },
                desktop: {
                    shape: 'dot',
                    color: "#2B7CE9" // blue
                },
                mobile: {
                    shape: 'dot',
                    color: "#5A1E5C" // purple
                },
                server: {
                    shape: 'square',
                    color: "#C5000B" // red
                },
                internet: {
                    shape: 'square',
                    color: "#109618" // green
                }
            }

        };

        network = new vis.Network(container, data, options);

        // add event listeners
        network.on('select', function(params) {
            document.getElementById('selection').innerHTML = 'Selection: ' + params.nodes;
        });

    }, function errorCallback(response) {
        // 请求失败执行代码
    });
});

//手动加载ng-app
angular.bootstrap(document.querySelector('[ng-app="runtimeApp"]'), ['runtimeApp']);
