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
        var my_slaves = new Array(5*$scope.quantities);

        var my_master = {};
        my_slaves[0] = my_master;
        my_master.id = 1;
        my_master.label = "master";
        my_master.shape = 'image';
        my_master.image = DIR + 'Network-Pipe-icon.png';
        my_master.title = '主节点';

        //my_slaves[0] = my_master;
        var index_slave = 1;
        var index_edge = 0;
        var index_subslave =777;
        var my_edges = [];

        if($scope.quantities>=2){
            my_edges = [$scope.quantities-1];
            for(var i in $scope.runtime){
                var slave = $scope.runtime[i];

                var temp_slave = {};
                temp_slave.label = "slave"+index_slave;
                temp_slave.id = index_slave+1;
                temp_slave.shape ='image';
                temp_slave.image = DIR + 'Hardware-My-Computer-3-icon.png';
                temp_slave.title = JSON.stringify(slave);
                my_slaves[index_slave]= temp_slave;
                var temp_edge = {};
                temp_edge.from = 1;
                temp_edge.to = temp_slave.id;
                temp_edge.arrows = 'to';
                my_edges[index_edge] = temp_edge;
                index_edge++;
                for(var j =0;j<$scope.quantities;j++)
                {

                    temp_slave.id = index_subslave+1;
                    temp_slave.group ='server';
                    temp_edge.from =index_slave+1;
                    temp_edge.to =index_subslave+1;
                    temp_edge.arrows = 'to';
                    my_slaves[index_slave+2]=temp_slave;

                    temp_slave.id = index_subslave+2;
                    temp_slave.group ='server';
                    temp_edge.from =index_slave+1;
                    temp_edge.to =index_subslave+2;
                    temp_edge.arrows = 'to';
                    my_slaves[index_slave+3]=temp_slave;

                    temp_slave.id = index_subslave+3;
                    temp_slave.group ='server';
                    temp_edge.from =index_slave+1;
                    temp_edge.to =index_subslave+3;
                    temp_edge.arrows = 'to';
                    my_slaves[index_slave+4]=temp_slave;

                    temp_slave.id = index_subslave+4;
                    temp_slave.group ='server';
                    temp_edge.from =index_slave+1;
                    temp_edge.to =index_subslave+4;
                    temp_edge.arrows = 'to';
                    my_slaves[index_slave+5]=temp_slave;

                }
                index_slave++;

            }
        }else{
            my_edges=[];
        }


        var nodes = new vis.DataSet(my_slaves);


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
