/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Jessicalllo  sherlock_vip@163.com
 * Date       ：18-11-26
 * Description：conrtollers.js
 */

(function() {
    'use strict';

    var chameleon_app = angular.module('Chameleon');
    /**
     * Function model  :  Represents the current location of the text range or cursor selected by the user
     * Author          :  Jessicallo
     * Date            :  2019-3-1
     * Funtion name    :  hasSelectedText
     * */
    function hasSelectedText() {
        if (window.getSelection) {            // 支持所有浏览器，除了IE9之前的版本
            var range = window.getSelection();
            return range.toString().length > 0;
        }
        return false;
    }

    function updateMasterState($scope, $timeout, master_state) {
        // Don't do anything if the state hasn't changed.
        if ($scope.master_state == master_state) {
            return true;     // Continue polling.
        }


    }

    function updateSuperMasterState($scope, $timeout, supermaster_state) {
        // Don't do anything if the state hasn't changed.
        if ($scope.supermaster_state == supermaster_state) {
            return true;     // Continue polling.
        }
    }

    //启动master后获取hostname的路径地址
    var leadingChameleonMasterURL = function(path){
        var address = location.hostname + ':' + '6060';
        return '//'+ address + path;

    };

    //启动supermaster后获取hostname的路径地址
    var leadingChameleonSuperMasterURL = function(path){
        var address = location.hostname + ':' + '7000';
        return '//'+ address + path;

    };

    //数据自动更新的Controller
    chameleon_app.controller('UpdateCtrl',function($scope,$http,$timeout){
        //每隔两秒，向.../master/runtime-resources路由发送一次请求
        var pollState = function() {
            $scope.delay = 2000;
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/runtime-resources')
            }).then(function successCallback(response) {
                $scope.master = {
                    runtime    : response.data.content,
                    quantities : response.data.quantity,
                };
            }, function errorCallback(response) {
                // 请求失败执行代码
            });
            $timeout(pollState, $scope.delay);
        };
        pollState();

    });

    //关闭所有节点的Controller
    chameleon_app.controller('ClusterOperationCtrl',function ($scope,$http) {
        $scope.stop_cluster = function () {
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/stop-cluster')
            }).then(function successCallback(response) {
                console.log(response);
            }, function errorCallback(response) {
                // 请求失败执行代码
            });
        }
    });

    //控制机器节点的Controller
    chameleon_app.controller('HardwareCtrl', function($scope,$route, $http,$timeout) {

        var pollState = function() {
            $scope.delay = 3600000; //one hour updates
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/hardware-resources')
            }).then(function successCallback(response) {

                $scope.master = {
                    hardware   : response.data.content,
                    quantities : response.data.quantity,
                };
            }, function errorCallback(response) {
                // 请求失败执行代码
            });
            $timeout(pollState, $scope.delay);
        };
        pollState();

    });

    //统计所有集群资源的Controller
    chameleon_app.controller('RuntimeCtrl', function($scope,$rootScope,$http) {
    });

    //获得当前framework使用的调度算法的Controller
    chameleon_app.controller('SchedulerCtrl', function($scope,$rootScope,$http) {
        $http({
            method: 'GET',
            url: leadingChameleonMasterURL('/master/get-scheduler')
        }).then(function successCallback(response) {

            $scope.master = {
                scheduler: response.data.content,
                //quantities: response.data.quantity,

            };
        }, function errorCallback(response) {
        });

        var flag;
        var updateSelected = function (action, name) {
            name = "" + name + "";
            console.log(name);
            if (action == 'true') {
                $rootScope.selected = name;
            }
        };
        $scope.updateSelection = function ($event, name) {
            var radio = $event.target;
            console.log(radio);
            var action = (radio.checked ? 'true' : 'false');
            updateSelected(action, name);
        };

        $scope.isSelected = function (name) {

            return ;
        };

        // $scope.getRequiremt = function () {
        //     var str = "";
        //     for (var i = 0; i<$scope.selected.length; i++){
        //         str += "," + $scope.selected[i];
        //     }
        //     if(str != ""){
        //         str = str.substring(1);
        //     }
        //     return str;
        // };

        console.log($scope.selected) ;

    });

    //改变调度算法的Controller
    chameleon_app.controller('ChangeSchedulerCtrl', function($scope,$uibModal) {
        var alert_message = "确认更换算法?";
        $scope.openModal = function() {
            var modalInstance = $uibModal.open({
                templateUrl : 'shutdown.html',
                controller : 'ChangeSchedulerInstanceCtrl',   //shutdown modal对应的Controller
                resolve : {
                    date : function() {           //date作为shutdown modal的controller传入的参数
                        return alert_message;     //用于传递数据
                    }
                }
            })
        }

    });

    chameleon_app.controller('ChangeSchedulerInstanceCtrl', function($scope,$rootScope,$uibModalInstance,$http, date) {

        $scope.date= date;
        $scope.ok = function() {
            $http({
                method: "POST",
                url: leadingChameleonMasterURL('/master/change-scheduler'),
                data:{name:$rootScope.selected},
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                transformRequest:function (obj) {
                    var str = [];
                    for(var p in obj){
                        str.push(encodeURIComponent(p) + "=" + encodeURIComponent(obj[p]));
                    }
                    return str.join("&");
                }
            }).success(function(req) {
                console.log(req);
                $uibModalInstance.dismiss('cancel');
            });

        };
        $scope.cancel = function() {
            $uibModalInstance.dismiss('cancel');
        }
    });


    //资源利用率饼图的Controller
    chameleon_app.controller('SlaveCtrl', function($scope,$rootScope, $http,$routeParams) {
        $scope.slave_uuid = $routeParams.slave_uuid;
        $scope.slave_id = $routeParams.slave_id;
        $scope.cpu_used = $routeParams.cpu_used;
        $scope.mem_used = $routeParams.mem_used;
        $scope.disk_used = $routeParams.disk_used;

        // 基于准备好的dom，初始化echarts实例
        var myChart = echarts.init(document.getElementById('cpu'));

        // 使用刚指定的配置项和数据显示图表。
        myChart.setOption({
            title: {
                text: 'cpu使用率',
                x:'center'
            },
            tooltip : {
                trigger: 'item',
                formatter: "{a} <br/>{b} :  ({d}%)"
            },
            legend: {
                orient: 'vertical',
                left: 'right',
                data: ['已使用','未使用']
            },
            series : [
                {
                    name: 'cpu使用率',
                    type: 'pie',
                    radius: '55%',
                    label: {
                        normal: {
                            formatter: '  {b|{b}：}  {per|{d}%}  ',
                            backgroundColor: '#eee',
                            borderColor: '#aaa',
                            borderWidth: 1,
                            borderRadius: 4,
                            rich: {
                                a: {
                                    color: '#999',
                                    lineHeight: 22,
                                    align: 'center'
                                },
                                hr: {
                                    borderColor: '#aaa',
                                    width: '100%',
                                    borderWidth: 0.5,
                                    height: 0
                                },
                                b: {
                                    fontSize: 16,
                                    lineHeight: 33
                                },
                                per: {
                                    color: '#eee',
                                    backgroundColor: '#334455',
                                    padding: [2, 4],
                                    borderRadius: 2
                                }
                            }
                        }
                    },
                    center: ['50%', '60%'],
                    data:[
                        {value:$scope.cpu_used, name:'已使用'},
                        {value:100-$scope.cpu_used, name:'未使用'},
                    ],
                    itemStyle: {
                        emphasis: {
                            shadowBlur: 10,
                            shadowOffsetX: 0,
                            shadowColor: 'rgba(0, 0, 0, 0.5)'
                        }
                    }

                }
            ]
        });

        var myChart = echarts.init(document.getElementById('mem'));

        // 使用刚指定的配置项和数据显示图表。
        myChart.setOption({
            title: {
                text: 'memory使用率',
                x:'center'
            },
            tooltip : {
                trigger: 'item',
                formatter: "{a} <br/>{b} :  ({d}%)"
            },
            legend: {
                orient: 'vertical',
                left: 'right',
                data: ['已使用','未使用']
            },
            series : [
                {
                    name: 'memory使用率',
                    type: 'pie',
                    radius: '55%',
                    label: {
                        normal: {
                            formatter: '  {b|{b}：}  {per|{d}%}  ',
                            backgroundColor: '#eee',
                            borderColor: '#aaa',
                            borderWidth: 1,
                            borderRadius: 4,
                            // shadowBlur:3,
                            // shadowOffsetX: 2,
                            // shadowOffsetY: 2,
                            // shadowColor: '#999',
                            // padding: [0, 7],
                            rich: {
                                a: {
                                    color: '#999',
                                    lineHeight: 22,
                                    align: 'center'
                                },
                                // abg: {
                                //     backgroundColor: '#333',
                                //     width: '100%',
                                //     align: 'right',
                                //     height: 22,
                                //     borderRadius: [4, 4, 0, 0]
                                // },
                                hr: {
                                    borderColor: '#aaa',
                                    width: '100%',
                                    borderWidth: 0.5,
                                    height: 0
                                },
                                b: {
                                    fontSize: 16,
                                    lineHeight: 33
                                },
                                per: {
                                    color: '#eee',
                                    backgroundColor: '#334455',
                                    padding: [2, 4],
                                    borderRadius: 2
                                }
                            }
                        }
                    },
                    center: ['50%', '60%'],
                    data:[
                        {value:$scope.mem_used, name:'已使用'},
                        {value:100-$scope.mem_used, name:'未使用'},
                    ],
                    itemStyle: {
                        emphasis: {
                            shadowBlur: 10,
                            shadowOffsetX: 0,
                            shadowColor: 'rgba(0, 0, 0, 0.5)'
                        }
                    }

                }
            ]
        });

        var myChart = echarts.init(document.getElementById('disk'));

        // 使用刚指定的配置项和数据显示图表。
        myChart.setOption({
            title: {
                text: 'disk使用率',
                x:'center'
            },
            tooltip : {
                trigger: 'item',
                formatter: "{a} <br/>{b} :  ({d}%)"
            },
            legend: {
                orient: 'vertical',
                left: 'right',
                data: ['已使用','未使用']
            },
            series : [
                {
                    name: 'disk使用率',
                    type: 'pie',
                    radius: '55%',
                    label: {
                        normal: {
                            formatter: '  {b|{b}：}  {per|{d}%}  ',
                            backgroundColor: '#eee',
                            borderColor: '#aaa',
                            borderWidth: 1,
                            borderRadius: 4,
                            rich: {
                                a: {
                                    color: '#999',
                                    lineHeight: 22,
                                    align: 'center'
                                },
                                hr: {
                                    borderColor: '#aaa',
                                    width: '100%',
                                    borderWidth: 0.5,
                                    height: 0
                                },
                                b: {
                                    fontSize: 16,
                                    lineHeight: 33
                                },
                                per: {
                                    color: '#eee',
                                    backgroundColor: '#334455',
                                    padding: [2, 4],
                                    borderRadius: 2
                                }
                            }
                        }
                    },
                    center: ['50%', '60%'],
                    data:[
                        {value:$scope.disk_used, name:'已使用'},
                        {value:100-$scope.disk_used, name:'未使用'},
                    ],
                    itemStyle: {
                        emphasis: {
                            shadowBlur: 10,
                            shadowOffsetX: 0,
                            shadowColor: 'rgba(0, 0, 0, 0.5)'
                        }
                    }

                }
            ]
        });

        var myChart = echarts.init(document.getElementById('swap'));

        // 使用刚指定的配置项和数据显示图表。
        myChart.setOption({
            title: {
                text: 'swap使用率',
                x:'center'
            },
            tooltip : {
                trigger: 'item',
                formatter: "{a} <br/>{b} :  ({d}%)"
            },
            legend: {
                orient: 'vertical',
                left: 'right',
                data: ['已使用','未使用']
            },
            series : [
                {
                    name: 'swap使用率',
                    type: 'pie',
                    radius: '55%',
                    label: {
                        normal: {
                            formatter: '  {b|{b}：}  {per|{d}%}  ',
                            backgroundColor: '#eee',
                            borderColor: '#aaa',
                            borderWidth: 1,
                            borderRadius: 4,
                            rich: {
                                a: {
                                    color: '#999',
                                    lineHeight: 22,
                                    align: 'center'
                                },
                                hr: {
                                    borderColor: '#aaa',
                                    width: '100%',
                                    borderWidth: 0.5,
                                    height: 0
                                },
                                b: {
                                    fontSize: 16,
                                    lineHeight: 33
                                },
                                per: {
                                    color: '#eee',
                                    backgroundColor: '#334455',
                                    padding: [2, 4],
                                    borderRadius: 2
                                }
                            }
                        }
                    },
                    center: ['50%', '60%'],
                    data:[
                        {value:$scope.cpu_used, name:'已使用'},
                        {value:100-$scope.cpu_used, name:'未使用'},
                    ],
                    itemStyle: {
                        emphasis: {
                            shadowBlur: 10,
                            shadowOffsetX: 0,
                            shadowColor: 'rgba(0, 0, 0, 0.5)'
                        }
                    }

                }
            ]
        });
    });

    //网络拓扑图的Controller
    chameleon_app.controller('TopologyCtrl', function($scope, $http) {

        $http({
            method: 'GET',
            url: leadingChameleonMasterURL('/master/runtime-resources')
        }).then(function successCallback(response) {
            $scope.master = {
                runtime    : response.data.content,
                quantities : response.data.quantity,
            };

            var DIR = '../icon/refresh-cl/';
            var vertexes = new Array();

            var my_master = {};
            vertexes[0] = my_master;
            my_master.id = 0;
            my_master.label = "master";
            my_master.shape = 'image';
            my_master.image = DIR + 'Hardware-WQN-main.png';
            //my_master.title = "主节点";    //unchangeable

            var index_slave = 1;
            var index_edge = -1;
            var my_edges = [];
            var cur_index = 0;

            if ($scope.master.quantities >= 1) {
                my_edges = [];
                for (var i in $scope.master.runtime) {
                    var temp_slave = {}; // 添加一个顶点
                    cur_index++; // 全局id
                    my_master.title = $scope.master.runtime[0].resources.slave_id+':6060';
                    temp_slave.label = $scope.master.runtime[i].slave_hostname;
                    temp_slave.id = cur_index;
                    temp_slave.shape = 'image';
                    temp_slave.image = DIR + 'Hardware-WQN-server.png';
                    temp_slave.title = $scope.master.runtime[i].resources.slave_id;
                    vertexes[cur_index] = temp_slave;   // cur_index 同时代表 顶点集合 my_slaves 的下标

                    var temp_edge = {};   // 添加一条边 master -> temp_slave
                    temp_edge.from = 0;
                    temp_edge.to = temp_slave.id;
                    temp_edge.arrows = 'to';
                    temp_edge.label = Math.round($scope.master.runtime[i].resources.net_usage.net_used * 100) / 100 + 'KiB/s';
                    index_edge++; // 边集合 my_edges 的下标
                    my_edges[index_edge] = temp_edge;

                    // 添加cpu节点
                    var temp_cpu = {};
                    cur_index++;
                    temp_cpu.label = "cpu";
                    temp_cpu.id = cur_index;
                    temp_cpu.group = 'server';
                    temp_cpu.title = Math.round($scope.master.runtime[i].resources.cpu_usage.cpu_used);
                    temp_cpu.value = Math.ceil(Math.round($scope.master.runtime[i].resources.cpu_usage.cpu_used) / 10);
                    vertexes[cur_index] = temp_cpu;
                    // 添加连接cpu节点的边, temp_slave -> temp_cpu
                    var edge_cpu = {};
                    index_edge++;
                    edge_cpu.from = temp_slave.id;
                    edge_cpu.to = temp_cpu.id;
                    edge_cpu.arrows = 'to';
                    my_edges[index_edge] = edge_cpu;

                    // 添加disk节点
                    var temp_disk = {};
                    cur_index++;
                    temp_disk.label = "disk";
                    temp_disk.id = cur_index;
                    temp_disk.group = 'switch';
                    temp_disk.title = Math.round(100 - $scope.master.runtime[i].resources.disk_usage.available_percent);
                    temp_disk.value = Math.ceil(Math.round(100 - $scope.master.runtime[i].resources.disk_usage.available_percent) / 10);
                    vertexes[cur_index] = temp_disk;
                    // 添加连接cpu节点的边, temp_slave -> temp_cpu
                    var edge_disk = {};
                    index_edge++;
                    edge_disk.from = temp_slave.id;
                    edge_disk.to = temp_disk.id;
                    edge_disk.arrows = 'to';
                    my_edges[index_edge] = edge_disk;

                    // 添加mem节点
                    var temp_mem = {};
                    cur_index++;
                    temp_mem.label = "mem";
                    temp_mem.id = cur_index;
                    temp_mem.group = 'desktop';
                    temp_mem.title = Math.round($scope.master.runtime[i].resources.mem_usage.mem_available / $scope.master.runtime[i].resources.mem_usage.mem_total * 100);
                    temp_mem.value = Math.ceil(Math.round($scope.master.runtime[i].resources.mem_usage.mem_available / $scope.master.runtime[i].resources.mem_usage.mem_total * 100) / 10);
                    vertexes[cur_index] = temp_mem;
                    // 添加连接mem节点的边, temp_slave -> temp_mem
                    var edge_mem = {};
                    index_edge++;
                    edge_mem.from = temp_slave.id;
                    edge_mem.to = temp_mem.id;
                    edge_mem.arrows = 'to';
                    my_edges[index_edge] = edge_mem;

                    // 添加swap节点
                    var temp_swap = {};
                    cur_index++;
                    temp_swap.label = "swap";
                    temp_swap.id = cur_index;
                    temp_swap.group = 'mobile';
                    temp_swap.title = $scope.master.runtime[i].resources.mem_usage.swap_free / $scope.master.runtime[i].resources.mem_usage.swap_total * 100;
                    vertexes[cur_index] = temp_swap;
                    // 添加连接net节点的边, temp_slave -> temp_net
                    var edge_swap = {};
                    index_edge++;
                    edge_swap.from = temp_slave.id;
                    edge_swap.to = temp_swap.id;
                    edge_swap.arrows = 'to';
                    my_edges[index_edge] = edge_swap;


                }
            } else {
                my_edges = [];
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
                        shape: 'dot',
                        color: '#FF9900' // orange
                    },
                    desktop: {
                        shape: 'dot',
                        color: "#109618" // green
                    },
                    mobile: {
                        shape: 'dot',
                        color: "#5A1E5C" // purple
                    },
                    server: {
                        shape: 'dot',
                        color: "#c53c3d" // red
                    },
                    internet: {
                        shape: 'square',
                        color: "#0c58c5" // blue
                    }
                }

            };

            var network = new vis.Network(container, data, options);

            // add event listeners
            network.on('select', function (params) {
                document.getElementById('selection').innerHTML = 'Selection: ' + params.nodes;
            });

        }, function errorCallback(response) {
            // 请求失败执行代码
        });

    });

    chameleon_app.controller('SuperTopologyCtrl', function($scope, $http){
        $http({
            method: 'GET',
            url: leadingChameleonSuperMasterURL('/super_master/resources')       //此API查看super_master管理的所有master及slave信息
        }).then(function successCallback(response) {
            $scope.master_slave = {
                content : response.data.content,
                master_quantity : response.data.master_quantity
            };
            // $scope.content = {
            //     master : response.data.content.master,
            //     slaves : response.data.content.slaves,
            //     slave_quantity : response.data.content.slave_quantity
            // }

            var DIR = '../icon/refresh-cl/';

            var vertexes = new Array()

            var my_supermaster = {};            //构造一个主super_master节点
            vertexes[0] = my_supermaster;
            my_supermaster.id = 0;
            my_supermaster.label = "super_master";
            my_supermaster.shape = 'image';
            my_supermaster.image = DIR + 'Hardware-WQN-superserver.png';
            my_supermaster.title = location.hostname + ':' + '7000';

            var index_edge = -1;
            var my_edges = [];
            var cur_index = 0;

            if ($scope.master_slave.master_quantity >= 1) {
                // my_edges = [];   //构造一条边

                for (var i in $scope.master_slave.content) {
                    var my_master = {};    //构造一个master节点
                    cur_index++;      //全局变量
                    my_master.id = cur_index;
                    my_master.label = "master";
                    my_master.shape = 'image';
                    my_master.image = DIR + 'Hardware-WQN-main.png';
                    vertexes[cur_index] = my_master;

                    my_master.title = $scope.master_slave.content[i].master;  // ip:port
                    var temp_superedge = {};       //添加一条super_master到my_master的边

                    temp_superedge.from = my_supermaster.id;
                    temp_superedge.to = my_master.id;
                    temp_superedge.arrows = 'to';
                    index_edge++;
                    my_edges[index_edge] = temp_superedge;

                    if ($scope.master_slave.content[i].slave_quantity >= 1) {  //slave的个数
                        for (var j in $scope.master_slave.content[i].slaves) {
                            var temp_slave = {};       // 添加一个slave节点
                            cur_index++;
                            //console.log('2' + cur_index);
                            temp_slave.id = cur_index;
                            temp_slave.label = $scope.master_slave.content[i].slaves[j].hardware_resources.slave_hostname;
                            temp_slave.shape = 'image';
                            temp_slave.image = DIR + 'Hardware-WQN-server.png';
                            temp_slave.title = $scope.master_slave.content[i].slaves[j].slave_ip;
                            vertexes[cur_index] = temp_slave;

                            var temp_edge = {};        // 添加一条边 master -> temp_slave
                            temp_edge.from = my_master.id;
                            temp_edge.to = temp_slave.id;
                            temp_edge.arrows = 'to';
                            index_edge++; // 边集合 my_edges 的下标
                            my_edges[index_edge] = temp_edge;

                            // 添加cpu节点
                            var temp_cpu = {};
                            cur_index++;
                            temp_cpu.label = "cpu";
                            temp_cpu.id = cur_index;
                            temp_cpu.group = 'server';
                            temp_cpu.title = Math.round($scope.master_slave.content[i].slaves[j].runtime_resources.cpu_usage.cpu_used);
                            temp_cpu.value = Math.ceil(Math.round($scope.master_slave.content[i].slaves[j].runtime_resources.cpu_usage.cpu_used) / 10);
                            vertexes[cur_index] = temp_cpu;
                            // 添加连接cpu节点的边, temp_slave -> temp_cpu
                            var edge_cpu = {};
                            index_edge++;
                            edge_cpu.from = temp_slave.id;
                            edge_cpu.to = temp_cpu.id;
                            edge_cpu.arrows = 'to';
                            my_edges[index_edge] = edge_cpu;

                            // 添加disk节点
                            var temp_disk = {};
                            cur_index++;
                            temp_disk.label = "disk";
                            temp_disk.id = cur_index;
                            temp_disk.group = 'switch';
                            temp_disk.title = Math.round(100 - $scope.master_slave.content[i].slaves[j].runtime_resources.disk_usage.available_percent);
                            temp_disk.value = Math.ceil(Math.round(100 - $scope.master_slave.content[i].slaves[j].runtime_resources.disk_usage.available_percent) / 10);
                            vertexes[cur_index] = temp_disk;
                            // 添加连接cpu节点的边, temp_slave -> temp_cpu
                            var edge_disk = {};
                            index_edge++;
                            edge_disk.from = temp_slave.id;
                            edge_disk.to = temp_disk.id;
                            edge_disk.arrows = 'to';
                            my_edges[index_edge] = edge_disk;

                            // 添加mem节点
                            var temp_mem = {};
                            cur_index++;
                            temp_mem.label = "mem";
                            temp_mem.id = cur_index;
                            temp_mem.group = 'desktop';
                            temp_mem.title = Math.round($scope.master_slave.content[i].slaves[j].runtime_resources.mem_usage.mem_available / $scope.master_slave.content[i].slaves[j].runtime_resources.mem_usage.mem_total * 100);
                            temp_mem.value = Math.ceil(Math.round($scope.master_slave.content[i].slaves[j].runtime_resources.mem_usage.mem_available / $scope.master_slave.content[i].slaves[j].runtime_resources.mem_usage.mem_total * 100) / 10);
                            vertexes[cur_index] = temp_mem;
                            // 添加连接mem节点的边, temp_slave -> temp_mem
                            var edge_mem = {};
                            index_edge++;
                            edge_mem.from = temp_slave.id;
                            edge_mem.to = temp_mem.id;
                            edge_mem.arrows = 'to';
                            my_edges[index_edge] = edge_mem;

                            // 添加swap节点
                            var temp_swap = {};
                            cur_index++;
                            temp_swap.label = "swap";
                            temp_swap.id = cur_index;
                            temp_swap.group = 'mobile';
                            temp_swap.title = $scope.master_slave.content[i].slaves[j].runtime_resources.mem_usage.swap_free / $scope.master_slave.content[i].slaves[j].runtime_resources.mem_usage.swap_total * 100;
                            vertexes[cur_index] = temp_swap;
                            // 添加连接net节点的边, temp_slave -> temp_net
                            var edge_swap = {};
                            index_edge++;
                            edge_swap.from = temp_slave.id;
                            edge_swap.to = temp_swap.id;
                            edge_swap.arrows = 'to';
                            my_edges[index_edge] = edge_swap;
                            }

                            var nodes = new vis.DataSet(vertexes);
                            var edges = new vis.DataSet(my_edges);

                            var data = {
                                nodes: nodes,
                                edges: edges
                            };
                            var container = document.getElementById('mynetwork');

                            var options = {
                                interaction: {
                                    navigationButtons: true,
                                    keyboard: true
                                },
                                groups: {
                                    'switch': {
                                        shape: 'dot',
                                        color: '#FF9900' // orange
                                    },
                                    desktop: {
                                        shape: 'dot',
                                        color: "#109618" // green
                                    },
                                    mobile: {
                                        shape: 'dot',
                                        color: "#5A1E5C" // purple
                                    },
                                    server: {
                                        shape: 'dot',
                                        color: "#c53c3d" // red
                                    },
                                    internet: {
                                        shape: 'square',
                                        color: "#0c58c5" // blue
                                    }
                                },

                            };

                            var network = new vis.Network(container, data, options);

                            // add event listeners
                            network.on('select', function (params) {
                                document.getElementById('selection').innerHTML = 'Selection: ' + params.nodes;
                            });

                        }


                    }
                }

        }, function errorCallback(response) {
            // 请求失败执行代码
        });

    });

    chameleon_app.controller('SuperAndSuperTopologyCtrl', function($scope, $http){      //可合并到下发的SuperAndSuperTopologyCtrl中
        $http({
            method: 'GET',
            url: leadingChameleonSuperMasterURL('/super_master/super_master')       //此API查看super_master管理的master,若请求成功则为两层及以上结构
        }).then(function successCallback(response) {

            $scope.supermaster = {
                contents   : response.data.content,
                quantities : response.data.quantity,
            };

            var DIR = '../icon/refresh-cl/';

            var vertexes_super = new Array()

            var my_supermaster = {};            //构造一个主super_master节点
            vertexes_super[0] = my_supermaster;
            my_supermaster.id = 0;
            my_supermaster.label = "super_master";
            my_supermaster.shape = 'image';
            my_supermaster.image = DIR + 'Hardware-WQN-superserver.png';
            my_supermaster.title = $scope.supermaster.contents[0]+':7000';　　//默认主super_master在本地运行

            var index_master = 1;
            var index_edge = -1;
            var my_superedges = [];
            var cur_masterindex = 0;

            if ($scope.supermaster.quantities >= 1) {
                // my_superedges = [];   //构造一条边

                for (var i in $scope.supermaster.contents) {
                    var my_master = {};    //构造一个master节点
                    vertexes_super[parseInt(i)+1] = my_master;
                    cur_masterindex++;      //全局变量
                    my_master.id = cur_masterindex;
                    my_master.label = "master";
                    my_master.shape = 'image';
                    my_master.image = DIR + 'Hardware-WQN-main.png';

                    my_master.title = $scope.supermaster.contents[i] + ':6060';
                    var temp_superedge = {};       //添加一条super_master到my_master的边

                    temp_superedge.from = my_supermaster.id;
                    temp_superedge.to = my_master.id;
                    temp_superedge.arrows = 'to';
                    index_edge++;
                    my_superedges[index_edge] = temp_superedge;
                    //
                    // alert(i);
//                     $http({
//                         method: 'GET',
//                         url: '//' + $scope.supermaster.contents[i] + ':6060/master/runtime-resources',//leadingChameleonMasterURL('/master/runtime-resources')
//                     }).then(function successCallback(response) {
//
//                         $scope.master = {
//                             runtime: response.data.content,
//                             quantities: response.data.quantity,
//                         };
//
//
//                         if ($scope.master.quantities >= 1) {        //实际上是slave的个数
//                            // alert(2);
// //                            alert($scope.master.quantities);
//                             for (var j in $scope.master.runtime) {
// //                                alert(j);
//                                 alert($scope.master.runtime[j].slave_id+i);
// //                                 alert(cur_masterindex);
//                                 var temp_slave = {};       // 添加一个slave节点
//                                 cur_masterindex++;
//                                 //console.log('2' + cur_masterindex);
//                                 temp_slave.id = cur_masterindex;
//                                 temp_slave.shape = 'image';
//                                 temp_slave.image = DIR + 'Hardware-WQN-server.png';
//                                 temp_slave.title = $scope.master.runtime[j].slave_id;
//                                 vertexes_super[cur_masterindex] = temp_slave;
//
//                                 // var temp_edge = {};        // 添加一条边 master -> temp_slave
//                                 // temp_edge.from = my_master.id;
//                                 // temp_edge.to = temp_slave.id;
//                                 // temp_edge.arrows = 'to';
//                                 // index_edge++; // 边集合 my_edges 的下标
//                                 // my_superedges[index_edge] = temp_edge;
//                                 //
//                                 // // 添加cpu节点
//                                 // var temp_cpu = {};
//                                 // cur_masterindex++;
//                                 // temp_cpu.label = "cpu";
//                                 // temp_cpu.id = cur_masterindex;
//                                 // temp_cpu.group = 'server';
//                                 // temp_cpu.title = Math.round($scope.master.runtime[j].cpu_usage.cpu_used);
//                                 // temp_cpu.value = Math.ceil(Math.round($scope.master.runtime[j].cpu_usage.cpu_used) / 10);
//                                 // vertexes_super[cur_masterindex] = temp_cpu;
//                                 // // 添加连接cpu节点的边, temp_slave -> temp_cpu
//                                 // var edge_cpu = {};
//                                 // index_edge++;
//                                 // edge_cpu.from = temp_slave.id;
//                                 // edge_cpu.to = temp_cpu.id;
//                                 // edge_cpu.arrows = 'to';
//                                 // my_superedges[index_edge] = edge_cpu;
//                                 //
//                                 // // 添加disk节点
//                                 // var temp_disk = {};
//                                 // cur_masterindex++;
//                                 // temp_disk.label = "disk";
//                                 // temp_disk.id = cur_masterindex;
//                                 // temp_disk.group = 'switch';
//                                 // temp_disk.title = Math.round(100 - $scope.master.runtime[j].disk_usage.available_percent);
//                                 // temp_disk.value = Math.ceil(Math.round(100 - $scope.master.runtime[j].disk_usage.available_percent) / 10);
//                                 // vertexes_super[cur_masterindex] = temp_disk;
//                                 // // 添加连接cpu节点的边, temp_slave -> temp_cpu
//                                 // var edge_disk = {};
//                                 // index_edge++;
//                                 // edge_disk.from = temp_slave.id;
//                                 // edge_disk.to = temp_disk.id;
//                                 // edge_disk.arrows = 'to';
//                                 // my_superedges[index_edge] = edge_disk;
//                                 //
//                                 // // 添加mem节点
//                                 // var temp_mem = {};
//                                 // cur_masterindex++;
//                                 // temp_mem.label = "mem";
//                                 // temp_mem.id = cur_masterindex;
//                                 // temp_mem.group = 'desktop';
//                                 // temp_mem.title = Math.round($scope.master.runtime[j].mem_usage.mem_available / $scope.master.runtime[j].mem_usage.mem_total * 100);
//                                 // temp_mem.value = Math.ceil(Math.round($scope.master.runtime[j].mem_usage.mem_available / $scope.master.runtime[j].mem_usage.mem_total * 100) / 10);
//                                 // vertexes_super[cur_masterindex] = temp_mem;
//                                 // // 添加连接mem节点的边, temp_slave -> temp_mem
//                                 // var edge_mem = {};
//                                 // index_edge++;
//                                 // edge_mem.from = temp_slave.id;
//                                 // edge_mem.to = temp_mem.id;
//                                 // edge_mem.arrows = 'to';
//                                 // my_superedges[index_edge] = edge_mem;
//                                 //
//                                 // // 添加swap节点
//                                 // var temp_swap = {};
//                                 // cur_masterindex++;
//                                 // temp_swap.label = "swap";
//                                 // temp_swap.id = cur_masterindex;
//                                 // temp_swap.group = 'mobile';
//                                 // temp_swap.title = $scope.master.runtime[j].mem_usage.swap_free / $scope.master.runtime[j].mem_usage.swap_total * 100;
//                                 // vertexes_super[cur_masterindex] = temp_swap;
//                                 // // 添加连接net节点的边, temp_slave -> temp_net
//                                 // var edge_swap = {};
//                                 // index_edge++;
//                                 // edge_swap.from = temp_slave.id;
//                                 // edge_swap.to = temp_swap.id;
//                                 // edge_swap.arrows = 'to';
//                                 // my_superedges[index_edge] = edge_swap;
//                             //}
//
                    var nodes = new vis.DataSet(vertexes_super);
                    var edges = new vis.DataSet(my_superedges);

                    var data = {
                        nodes: nodes,
                        edges: edges
                    };
                    var container = document.getElementById('mynetwork');

                    var options = {
                        interaction: {
                            navigationButtons: true,
                            keyboard: true
                        },
                        groups: {
                            'switch': {
                                shape: 'dot',
                                color: '#FF9900' // orange
                            },
                            desktop: {
                                shape: 'dot',
                                color: "#109618" // green
                            },
                            mobile: {
                                shape: 'dot',
                                color: "#5A1E5C" // purple
                            },
                            server: {
                                shape: 'dot',
                                color: "#c53c3d" // red
                            },
                            internet: {
                                shape: 'square',
                                color: "#0c58c5" // blue
                            }
                        },

                    };

                    var network = new vis.Network(container, data, options);

                    // add event listeners
                    network.on('select', function (params) {
                        document.getElementById('selection').innerHTML = 'Selection: ' + params.nodes;
                    });
//
//                             }
//
//
//                         }
//
//                     }, function errorCallback(response) {
//                         // 请求失败执行代码
//                     });
                    //
                }


            }


        }, function errorCallback(response) {
            // 请求失败执行代码
        });

    });

    //framework拓扑图的Controller
    chameleon_app.controller('FrameworkTopologyCtrl', function ($scope, $http, $timeout) {
        var pollState = function () {
            $scope.delay = 10000;
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/runtime-resources')
            }).then(function successCallback(response) {

                $scope.master = {
                    runtime: response.data.content,
                    quantities: response.data.quantity,
                };

                let DIR = '../icon/refresh-cl/';
                var vertexes = new Array();

                var my_master = {};
                vertexes[0] = my_master;
                my_master.id = 0;
                my_master.label = "master";
                my_master.shape = 'image';
                my_master.image = DIR + 'Hardware-WQN-main.png';
                //my_master.title = "主节点";    //unchangeable

                var index_slave = 1;
                var index_edge = -1;
                var my_edges = [];
                var cur_index = 0;

                if ($scope.master.quantities >= 1) {
                    my_edges = [];
                    for (var i in $scope.master.runtime) {
                        var slave = $scope.master.runtime[i];

                        var temp_slave = {}; // 添加一个顶点
                        cur_index++;         // 全局id

                        my_master.title = $scope.master.runtime[0].resources.slave_id + ':6060';   //master节点需要用到slave的消息，所以写在这里
                        temp_slave.label = $scope.master.runtime[i].slave_hostname;
                        temp_slave.id = cur_index;
                        temp_slave.shape = 'image';
                        temp_slave.image = DIR + 'Hardware-WQN-server.png';
                        temp_slave.title = $scope.master.runtime[i].resources.slave_id;
                        vertexes[cur_index] = temp_slave; // cur_index 同时代表 顶点集合 my_slaves 的下标

                        var temp_edge = {}; // 添加一条边 master -> temp_slave
                        temp_edge.from = 0;
                        temp_edge.to = temp_slave.id;
                        temp_edge.arrows = 'to';
                        temp_edge.label = Math.round($scope.master.runtime[i].resources.net_usage.net_used * 100) / 100 + 'KiB/s';
                        index_edge++; // 边集合 my_edges 的下标
                        my_edges[index_edge] = temp_edge;


                    }
                } else {
                    my_edges = [];
                }

                $http({
                    method: 'GET',
                    url: leadingChameleonMasterURL('/master/frameworks')
                }).then(function successCallback(response) {

                    $scope.master = {
                        framework:
                            {
                                resource: response.data.content,
                                quantities: response.data.quantity,
                            }
                    };

                    if ($scope.master.framework.quantities >= 1) {
                        //var my_edges = [];
                        var framework_index = cur_index;
                        for (var f in $scope.master.framework.resource) {
                            //   添加framework运行节点
                            var temp_framework = {};
                            framework_index++;
                            temp_framework.label = $scope.master.framework.resource[f].name;
                            // temp_slave.label = $scope.master.framework.resource[f].hostname;
                            temp_framework.id = framework_index;
                            temp_framework.shape = 'image';
                            temp_framework.image = DIR + 'Hardware-WQN-spark.png'
                            temp_framework.title = $scope.master.framework.resource[f].id;
                            // temp_framework.value = Math.ceil(Math.round($scope.master_runtime[i].cpu_usage.cpu_used) / 10);
                            vertexes[framework_index] = temp_framework;
                            // 添加连接cpu节点的边, temp_slave -> temp_cpu
                            var edge_framework = {};
                            index_edge++;
                            edge_framework.from = temp_framework.id;
                            edge_framework.to = my_master.id;
                            edge_framework.arrows = 'to';
                            my_edges[index_edge] = edge_framework;
                        }

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
                    };

                    var network = new vis.Network(container, data, options);

                    // add event listeners
                    network.on('select', function (params) {
                        document.getElementById('selection').innerHTML = 'Selection: ' + params.nodes;
                    });


                }, function errorCallback(response) {
                    // 请求失败执行代码
                });


            }, function errorCallback(response) {
                // 请求失败执行代码
            });
            $timeout(pollState, $scope.delay);
        };
        pollState();
    });

    //模态框对应的Controller
    chameleon_app.controller('ShutdownCtrl', function($scope,$uibModal) {
        var alert_message = "确认关闭所有slave节点?";
        $scope.openModal = function() {
            var modalInstance = $uibModal.open({
                templateUrl : 'shutdown.html',
                controller : 'ShutdownInstanceCtrl',   //shutdown modal对应的Controller
                resolve : {
                    date : function() {           //date作为shutdown modal的controller传入的参数
                        return alert_message;     //用于传递数据
                    }
                }
            })
        }
    });

    //模态框对应的Controller
    chameleon_app.controller('ShutdownInstanceCtrl', function($scope, $uibModalInstance,$http, date) {
        $scope.date= date;

        //在这里处理要进行的操作
        $scope.ok = function() {
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/stop-cluster')
            }).then(function successCallback(response) {
                console.log(response);
                $uibModalInstance.dismiss('cancel');
            }, function errorCallback(response) {
                // 请求失败执行代码
            });
        };
        $scope.cancel = function() {
            $uibModalInstance.dismiss('cancel');
        }
    });

    var SupermasterStarted;
    //开启两层supermaster对应的Controller
    chameleon_app.controller('StartSupermasterCtrl', function ($scope, $rootScope, $uibModal) {
        //$scope.startSupermaster = response.data.start;
        console.log('startSupermaster' + $rootScope.startSupermaster);
        console.log('stopSupermaster'+ $rootScope.stopSupermaster);
        $scope.openModal = function () {
            if ($rootScope.stopSupermaster == 'success'||$rootScope.startSupermaster == undefined) {
                //$scope.startSupermaster = $rootScope.startSupermaster;
                var alert_message = "确认开启Supermaster?";
                console.log('after alert startSupermaster' + $rootScope.startSupermaster);
                var modalInstance = $uibModal.open({
                    templateUrl: 'ControlSupermaster.html',
                    controller: 'StartSupermasterInstanceCtrl',   //shutdown modal对应的Controller
                    resolve: {
                        date: function () {                //date作为shutdown modal的controller传入的参数
                            return alert_message;          //用于传递数据
                        }
                    }
                })
            }
            else if ($rootScope.startSupermaster == 'success'){
                $scope.openModal = function () {
                    alert("supermaster已经开启");

                }
            }

        }
    });

    chameleon_app.controller('StartSupermasterInstanceCtrl', function($scope,$rootScope,$routeParams, $uibModalInstance,$http, date) {
        $scope.date= date;

        //在这里处理要进行的操作
        $scope.ok = function() {
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/start_supermaster')
            }).then(function successCallback(response) {
                $rootScope.startSupermaster = response.data.start;
                if($rootScope == 'success'){
                    SupermasterStarted = true;
                }
                else
                    SupermasterStarted = false;
                console.log('after confirm start'+$rootScope.startSupermaster);
                $uibModalInstance.dismiss('cancel');

            }, function errorCallback(response) {
                // 请求失败执行代码
            });
        };
        $scope.cancel = function() {
            $uibModalInstance.dismiss('cancel');
        }
    });

    //开启三层supermaster对应的Controller
    chameleon_app.controller('StartThreeSupermasterCtrl', function ($scope, $rootScope, $uibModal) {
        $scope.openModal = function () {
            if ($rootScope.startSupermaster == undefined || SupermasterStarted == false) {
                var alert_message = "确认开启Supermaster?";
                var modalInstance = $uibModal.open({
                    templateUrl: 'ControlSupermaster.html',
                    controller: 'StartThreeSupermasterInstanceCtrl',   //shutdown modal对应的Controller
                    resolve: {
                        date: function () {                //date作为shutdown modal的controller传入的参数
                            return alert_message;          //用于传递数据
                        }
                    }
                })
            }
            else {
                $scope.openModal = function () {
                    alert("supermaster已经开启");

                }
            }

        }
    });

    chameleon_app.controller('StartThreeSupermasterInstanceCtrl', function($scope,$rootScope,$routeParams, $uibModalInstance,$http, date) {
        $scope.date= date;

        //在这里处理要进行的操作
        $scope.ok = function() {
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/start_three_supermaster')
            }).then(function successCallback(response) {
                $rootScope.startSupermaster = response.data.start;
                console.log('10'+$rootScope.startSupermaster);
                $uibModalInstance.dismiss('cancel');
            }, function errorCallback(response) {
                // 请求失败执行代码
            });
        };
        $scope.cancel = function() {
            $uibModalInstance.dismiss('cancel');
        }
    });

    //关闭supermaster对应的Controller
    chameleon_app.controller('StopSupermasterCtrl',function ($scope,$rootScope,$uibModal) {
        console.log('stopSupermaster' + $rootScope.stopSupermaster);

        var alert_message = "确认停止Supermaster?";
        $scope.openModal = function() {
            if ($rootScope.stopSupermaster == undefined)
            {
                var modalInstance = $uibModal.open({
                    templateUrl: 'ControlSupermaster.html',
                    controller: 'StopSupermasterInstanceCtrl',   //shutdown modal对应的Controller
                    resolve: {
                        date: function () {                //date作为shutdown modal的controller传入的参数
                            return alert_message;          //用于传递数据
                        }
                    }
                })
            }
            else{
                $scope.openModal = function () {
                    alert("supermaster已经关闭");

                }
            }

        }

    });

    chameleon_app.controller('StopSupermasterInstanceCtrl', function($scope,$rootScope, $uibModalInstance,$http, date) {
        $scope.date= date;

        //在这里处理要进行的操作
        $scope.ok = function() {
            $http({
                method: 'GET',
                url: leadingChameleonSuperMasterURL('/super_master/kill_super_master')
            }).then(function successCallback(response) {
                $rootScope.stopSupermaster = response.data.stop;
                console.log(response);
                console.log('55' + $rootScope.stopSupermaster);
                $uibModalInstance.dismiss('cancel');

            }, function errorCallback(response) {
                // 请求失败执行代码
            });
        };
        $scope.cancel = function() {
            $uibModalInstance.dismiss('cancel');
        }
    });

    //spark框架对应的Controller
    chameleon_app.controller('FrameworksCtrl', function() {});

    chameleon_app.controller('FrameworkCtrl',function ($scope, $http, $timeout,$routeParams) {

        var pollState = function() {
            $scope.delay = 1000;
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/frameworks')
            }).then(function successCallback(response) {

                $scope.master = {
                    framework:
                        {
                            resource  : response.data.content,
                            quantities: response.data.quantity,
                        }
                };
                for (var f in $scope.master.framework.resource)
                {
                    for (var g in $scope.master.framework.resource[f].slaves) {
                        $scope.slaves =$scope.master.framework.resource[f].slaves;
                        //console.log($scope.slaves);
                    }
                }

                $scope.framework_uuid = $routeParams.framework_uuid;
            }, function errorCallback(response) {
            });
            $timeout(pollState, $scope.delay);
        };
        pollState();
    });

    chameleon_app.controller('AddStyleCtrl',function($scope)
    {
        $scope.tagcategories = [
            {
                id: 1,
                name: 'Color',
                tags: [
                    {
                        id:1,
                        name:'color1'
                    },
                    {
                        id:2,
                        name:'color2'
                    },
                    {
                        id:3,
                        name:'color3'
                    },
                    {
                        id:4,
                        name:'color4'
                    },
                ]
            },
            {
                id:2,
                name:'Cat',
                tags:[
                    {
                        id:5,
                        name:'cat1'
                    },
                    {
                        id:6,
                        name:'cat2'
                    },
                ]
            },
            {
                id:3,
                name:'Scenario',
                tags:[
                    {
                        id:7,
                        name:'Home'
                    },
                    {
                        id:8,
                        name:'Work'
                    },
                ]
            }
        ];

        $scope.selected = [];
        $scope.selectedTags = [];

        var updateSelected = function(action,id,name){
            if(action == 'add' && $scope.selected.indexOf(id) == -1){
                $scope.selected.push(id);
                $scope.selectedTags.push(name);
            }
            if(action == 'remove' && $scope.selected.indexOf(id)!=-1){
                var idx = $scope.selected.indexOf(id);
                $scope.selected.splice(idx,1);
                $scope.selectedTags.splice(idx,1);
            }
        }

        $scope.updateSelection = function($event, id){
            var checkbox = $event.target;
            var action = (checkbox.checked?'add':'remove');
            updateSelected(action,id,checkbox.name);
        }

        $scope.isSelected = function(id){
            return $scope.selected.indexOf(id)>=0;
        }
    });

})();