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

    var leadingChameleonMasterURL = function(path){
        var address = location.hostname + ':' + '6060';
        return '//'+ address + path;

    };

    var leadingChameleonSuperMasterURL = function(path){
        var address = location.hostname + ':' + '7000';
        return '//'+ address + path;

    };

    //数据自动更新的Controller
    chameleon_app.controller('UpdateCtrl',function($scope,$http,$timeout){

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

    chameleon_app.controller('HardwareCtrl', function($scope,$route, $http,$timeout) {

        var pollState = function() {
            $scope.delay = 3600000;
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

    chameleon_app.controller('RuntimeCtrl', function($scope,$rootScope,$http) {
    });

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
    });

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

    chameleon_app.controller('ChangeSchedulerInstanceCtrl', function($scope, $uibModalInstance,$http, date) {

        $scope.date= date;

        //在这里处理要进行的操作
        $scope.ok = function() {
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/change-scheduler')
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

    //拓扑图的Controller
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
                    my_master.title = $scope.master.runtime[0].slave_id+':6060';
                    temp_slave.label = "slave" + cur_index;
                    temp_slave.id = cur_index;
                    temp_slave.shape = 'image';
                    temp_slave.image = DIR + 'Hardware-WQN-server.png';
                    temp_slave.title = $scope.master.runtime[i].slave_id;
                    vertexes[cur_index] = temp_slave;   // cur_index 同时代表 顶点集合 my_slaves 的下标

                    var temp_edge = {};   // 添加一条边 master -> temp_slave
                    temp_edge.from = 0;
                    temp_edge.to = temp_slave.id;
                    temp_edge.arrows = 'to';
                    temp_edge.label = Math.round($scope.master.runtime[i].net_usage.net_used * 100) / 100 + 'KiB/s';
                    index_edge++; // 边集合 my_edges 的下标
                    my_edges[index_edge] = temp_edge;

                    // 添加cpu节点
                    var temp_cpu = {};
                    cur_index++;
                    temp_cpu.label = "cpu";
                    temp_cpu.id = cur_index;
                    temp_cpu.group = 'server';
                    temp_cpu.title = Math.round($scope.master.runtime[i].cpu_usage.cpu_used);
                    temp_cpu.value = Math.ceil(Math.round($scope.master.runtime[i].cpu_usage.cpu_used) / 10);
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
                    temp_disk.title = Math.round(100 - $scope.master.runtime[i].disk_usage.available_percent);
                    temp_disk.value = Math.ceil(Math.round(100 - $scope.master.runtime[i].disk_usage.available_percent) / 10);
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
                    temp_mem.title = Math.round($scope.master.runtime[i].mem_usage.mem_available / $scope.master.runtime[i].mem_usage.mem_total * 100);
                    temp_mem.value = Math.ceil(Math.round($scope.master.runtime[i].mem_usage.mem_available / $scope.master.runtime[i].mem_usage.mem_total * 100) / 10);
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
                    temp_swap.title = $scope.master.runtime[i].mem_usage.swap_free / $scope.master.runtime[i].mem_usage.swap_total * 100;
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
            url: leadingChameleonSuperMasterURL('/super_master/super_master')
        }).then(function successCallback(response) {

            $scope.supermaster = {
                resource   : response.data.content,
                quantities : response.data.quantity,
            };

            var DIR = '../icon/refresh-cl/';

            var vertexes_super = new Array()

            var my_supermaster = {};            //构造一个super_master节点
            vertexes_super[0] = my_supermaster;
            my_supermaster.id = 0;
            my_supermaster.label = "super_master";
            my_supermaster.shape = 'image';
            my_supermaster.image = DIR + 'Hardware-WQN-superserver.png';
            my_supermaster.title = $scope.supermaster.resource[0].ip+':7000';

            var index_master = 1;
            var index_superedge = -1;
            var my_superedges = [];
            var cur_masterindex = 0;

            if ($scope.supermaster.quantities >= 1) {
                my_superedges = [];   //构造一条边
                $http({
                    method: 'GET',
                    url: leadingChameleonMasterURL('/master/runtime-resources')
                }).then(function successCallback(response) {

                    $scope.master = {
                        runtime    : response.data.content,
                        quantities : response.data.quantity,
                    };

                    for (var i in $scope.supermaster.resource) {
                        var my_master = {};    //构造一个master节点
                        vertexes_super[0 + $scope.supermaster.quantities] = my_master;
                        cur_masterindex++;      //全局变量
                        my_master.id = cur_masterindex;
                        my_master.label = "master";
                        my_master.shape = 'image';
                        my_master.image = DIR + 'Hardware-WQN-main.png';
                        my_master.title = $scope.master.runtime[i].slave_id +':6060';
                        var temp_superedge = {};       //添加一条super_master到my_master的边

                        temp_superedge.from = my_supermaster.id;
                        temp_superedge.to = my_master.id;
                        temp_superedge.arrows = 'to';
                        index_superedge++;
                        my_superedges[index_superedge] = temp_superedge;
                    }
                    if ($scope.master.quantities >= 1) {
                        for (var j in $scope.master.runtime) {
                            var temp_slave = {};       // 添加一个slave节点
                            cur_masterindex++;
                            //console.log('2' + cur_masterindex);
                            temp_slave.id = cur_masterindex;
                            temp_slave.shape = 'image';
                            temp_slave.image = DIR + 'Hardware-WQN-server.png';
                            temp_slave.title = $scope.master.runtime[i].slave_id;
                            vertexes_super[cur_masterindex] = temp_slave;

                            var temp_edge = {};        // 添加一条边 master -> temp_slave
                            temp_edge.from = my_master.id;
                            temp_edge.to = temp_slave.id;
                            temp_edge.arrows = 'to';
                            index_superedge++; // 边集合 my_edges 的下标
                            my_superedges[index_superedge] = temp_edge;

                            // 添加cpu节点
                            var temp_cpu = {};
                            cur_masterindex++;
                            temp_cpu.label = "cpu";
                            temp_cpu.id = cur_masterindex;
                            temp_cpu.group = 'server';
                            temp_cpu.title = Math.round($scope.master.runtime[i].cpu_usage.cpu_used);
                            temp_cpu.value = Math.ceil(Math.round($scope.master.runtime[i].cpu_usage.cpu_used) / 10);
                            vertexes_super[cur_masterindex] = temp_cpu;
                            // 添加连接cpu节点的边, temp_slave -> temp_cpu
                            var edge_cpu = {};
                            index_superedge++;
                            edge_cpu.from = temp_slave.id;
                            edge_cpu.to = temp_cpu.id;
                            edge_cpu.arrows = 'to';
                            my_superedges[index_superedge] = edge_cpu;

                            // 添加disk节点
                            var temp_disk = {};
                            cur_masterindex++;
                            temp_disk.label = "disk";
                            temp_disk.id = cur_masterindex;
                            temp_disk.group = 'switch';
                            temp_disk.title = Math.round(100 - $scope.master.runtime[i].disk_usage.available_percent);
                            temp_disk.value = Math.ceil(Math.round(100 - $scope.master.runtime[i].disk_usage.available_percent) / 10);
                            vertexes_super[cur_masterindex] = temp_disk;
                            // 添加连接cpu节点的边, temp_slave -> temp_cpu
                            var edge_disk = {};
                            index_superedge++;
                            edge_disk.from = temp_slave.id;
                            edge_disk.to = temp_disk.id;
                            edge_disk.arrows = 'to';
                            my_superedges[index_superedge] = edge_disk;

                            // 添加mem节点
                            var temp_mem = {};
                            cur_masterindex++;
                            temp_mem.label = "mem";
                            temp_mem.id = cur_masterindex;
                            temp_mem.group = 'desktop';
                            temp_mem.title = Math.round($scope.master.runtime[i].mem_usage.mem_available / $scope.master.runtime[i].mem_usage.mem_total * 100);
                            temp_mem.value = Math.ceil(Math.round($scope.master.runtime[i].mem_usage.mem_available / $scope.master.runtime[i].mem_usage.mem_total * 100) / 10);
                            vertexes_super[cur_masterindex] = temp_mem;
                            // 添加连接mem节点的边, temp_slave -> temp_mem
                            var edge_mem = {};
                            index_superedge++;
                            edge_mem.from = temp_slave.id;
                            edge_mem.to = temp_mem.id;
                            edge_mem.arrows = 'to';
                            my_superedges[index_superedge] = edge_mem;

                            // 添加swap节点
                            var temp_swap = {};
                            cur_masterindex++;
                            temp_swap.label = "swap";
                            temp_swap.id = cur_masterindex;
                            temp_swap.group = 'mobile';
                            temp_swap.title = $scope.master.runtime[i].mem_usage.swap_free / $scope.master.runtime[i].mem_usage.swap_total * 100;
                            vertexes_super[cur_masterindex] = temp_swap;
                            // 添加连接net节点的边, temp_slave -> temp_net
                            var edge_swap = {};
                            index_superedge++;
                            edge_swap.from = temp_slave.id;
                            edge_swap.to = temp_swap.id;
                            edge_swap.arrows = 'to';
                            my_superedges[index_superedge] = edge_swap;
                        }

                    }

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

                }, function errorCallback(response) {
                    // 请求失败执行代码
                });
            }


        }, function errorCallback(response) {
            // 请求失败执行代码
        });

    });

    chameleon_app.controller('SuperAndSuperTopologyCtrl', function($scope, $http){
        $http({
            method: 'GET',
            url: leadingChameleonSuperMasterURL('/super_master/super_master')
        }).then(function successCallback(response) {

            $scope.supermaster = {
                resource   : response.data.content,
                quantities : response.data.quantity,
            };

            var DIR = '../icon/refresh-cl/';

            var vertexes_super = new Array()

            var my_supermaster = {};            //构造一个super_master节点
            vertexes_super[0] = my_supermaster;
            my_supermaster.id = 0;
            my_supermaster.label = "super_master";
            my_supermaster.shape = 'image';
            my_supermaster.image = DIR + 'Hardware-WQN-superserver.png';
            my_supermaster.title = $scope.supermaster.resource[0].ip+':7000';

            var index_master = 1;
            var index_superedge = -1;
            var my_superedges = [];
            var cur_masterindex = 0;

            if ($scope.supermaster.quantities >= 1) {
                // my_superedges = [];   //构造一条边
                // three
                $http({
                    method: 'GET',
                    url: leadingChameleonSuperMasterURL('/super_supermaster')
                }).then(function successCallback(response) {
                    $scope.super_master = {
                        contains   : response.data.content,
                        quantities : response.data.quantity,
                    };
                    for (var i in $scope.super_master.contains) {
                        var my_super_master = {};    //构造一个super_master节点
                        vertexes_super[0 + $scope.super_master.quantities] = my_super_master;
                        cur_masterindex++;      //全局变量,新
                        my_super_master.id = cur_masterindex;
                        my_super_master.label = "super_master";
                        my_super_master.shape = 'image';
                        my_super_master.image = DIR + 'Hardware-WQN-superserver.png';
                        my_super_master.title = $scope.super_master.contains[0].ip+':7001';//$scope.master.runtime[i].slave_id + ':6060';
                        var temp_superedge = {};       //添加一条super_master到my_super_master的边

                        temp_superedge.from = my_supermaster.id;
                        temp_superedge.to = my_super_master.id;
                        temp_superedge.arrows = 'to';
                        index_superedge++;
                        my_superedges[index_superedge] = temp_superedge;
                    }

                    if ($scope.super_master.quantities >= 1)
                    {
                        $http({
                            method: 'GET',
                            url: leadingChameleonMasterURL('/master/runtime-resources')
                        }).then(function successCallback(response) {

                            $scope.master = {
                                runtime: response.data.content,
                                quantities: response.data.quantity,
                            };

                            for (var i in $scope.supermaster.resource) {
                                var my_master = {};    //构造一个master节点
                                vertexes_super[$scope.super_master.quantities + $scope.supermaster.quantities] = my_master;
                                cur_masterindex++;      //全局变量
                                my_master.id = cur_masterindex;
                                my_master.label = "master";
                                my_master.shape = 'image';
                                my_master.image = DIR + 'Hardware-WQN-main.png';
                                my_master.title = $scope.master.runtime[i].slave_id + ':6060';
                                var temp_superedge = {};       //添加一条super_master到my_master的边

                                temp_superedge.from = my_super_master.id;
                                temp_superedge.to = my_master.id;
                                temp_superedge.arrows = 'to';
                                index_superedge++;
                                my_superedges[index_superedge] = temp_superedge;
                            }
                            if ($scope.master.quantities >= 1) {
                                for (var j in $scope.master.runtime) {
                                    var temp_slave = {};       // 添加一个slave节点
                                    cur_masterindex++;
                                    //console.log('2' + cur_masterindex);
                                    temp_slave.id = cur_masterindex;
                                    temp_slave.shape = 'image';
                                    temp_slave.image = DIR + 'Hardware-WQN-server.png';
                                    temp_slave.title = $scope.master.runtime[i].slave_id;
                                    vertexes_super[cur_masterindex] = temp_slave;

                                    var temp_edge = {};        // 添加一条边 master -> temp_slave
                                    temp_edge.from = my_master.id;
                                    temp_edge.to = temp_slave.id;
                                    temp_edge.arrows = 'to';
                                    index_superedge++; // 边集合 my_edges 的下标
                                    my_superedges[index_superedge] = temp_edge;

                                    // 添加cpu节点
                                    var temp_cpu = {};
                                    cur_masterindex++;
                                    temp_cpu.label = "cpu";
                                    temp_cpu.id = cur_masterindex;
                                    temp_cpu.group = 'server';
                                    temp_cpu.title = Math.round($scope.master.runtime[i].cpu_usage.cpu_used);
                                    temp_cpu.value = Math.ceil(Math.round($scope.master.runtime[i].cpu_usage.cpu_used) / 10);
                                    vertexes_super[cur_masterindex] = temp_cpu;
                                    // 添加连接cpu节点的边, temp_slave -> temp_cpu
                                    var edge_cpu = {};
                                    index_superedge++;
                                    edge_cpu.from = temp_slave.id;
                                    edge_cpu.to = temp_cpu.id;
                                    edge_cpu.arrows = 'to';
                                    my_superedges[index_superedge] = edge_cpu;

                                    // 添加disk节点
                                    var temp_disk = {};
                                    cur_masterindex++;
                                    temp_disk.label = "disk";
                                    temp_disk.id = cur_masterindex;
                                    temp_disk.group = 'switch';
                                    temp_disk.title = Math.round(100 - $scope.master.runtime[i].disk_usage.available_percent);
                                    temp_disk.value = Math.ceil(Math.round(100 - $scope.master.runtime[i].disk_usage.available_percent) / 10);
                                    vertexes_super[cur_masterindex] = temp_disk;
                                    // 添加连接cpu节点的边, temp_slave -> temp_cpu
                                    var edge_disk = {};
                                    index_superedge++;
                                    edge_disk.from = temp_slave.id;
                                    edge_disk.to = temp_disk.id;
                                    edge_disk.arrows = 'to';
                                    my_superedges[index_superedge] = edge_disk;

                                    // 添加mem节点
                                    var temp_mem = {};
                                    cur_masterindex++;
                                    temp_mem.label = "mem";
                                    temp_mem.id = cur_masterindex;
                                    temp_mem.group = 'desktop';
                                    temp_mem.title = Math.round($scope.master.runtime[i].mem_usage.mem_available / $scope.master.runtime[i].mem_usage.mem_total * 100);
                                    temp_mem.value = Math.ceil(Math.round($scope.master.runtime[i].mem_usage.mem_available / $scope.master.runtime[i].mem_usage.mem_total * 100) / 10);
                                    vertexes_super[cur_masterindex] = temp_mem;
                                    // 添加连接mem节点的边, temp_slave -> temp_mem
                                    var edge_mem = {};
                                    index_superedge++;
                                    edge_mem.from = temp_slave.id;
                                    edge_mem.to = temp_mem.id;
                                    edge_mem.arrows = 'to';
                                    my_superedges[index_superedge] = edge_mem;

                                    // 添加swap节点
                                    var temp_swap = {};
                                    cur_masterindex++;
                                    temp_swap.label = "swap";
                                    temp_swap.id = cur_masterindex;
                                    temp_swap.group = 'mobile';
                                    temp_swap.title = $scope.master.runtime[i].mem_usage.swap_free / $scope.master.runtime[i].mem_usage.swap_total * 100;
                                    vertexes_super[cur_masterindex] = temp_swap;
                                    // 添加连接net节点的边, temp_slave -> temp_net
                                    var edge_swap = {};
                                    index_superedge++;
                                    edge_swap.from = temp_slave.id;
                                    edge_swap.to = temp_swap.id;
                                    edge_swap.arrows = 'to';
                                    my_superedges[index_superedge] = edge_swap;
                                }

                            }

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

                        }, function errorCallback(response) {
                            // 请求失败执行代码
                        });
                    }
                    // three
                }, function errorCallback(response) {
                    // 请求失败执行代码
                });
            }


        }, function errorCallback(response) {
            // 请求失败执行代码
        });

    });

    chameleon_app.controller('FrameworkTopologyCtrl',function ($scope, $http,$timeout)
    {
        var pollState = function() {
            $scope.delay = 10000;
            $http({
                method: 'GET',
                url: leadingChameleonMasterURL('/master/runtime-resources')
            }).then(function successCallback(response) {

                $scope.master = {
                    runtime    : response.data.content,
                    quantities : response.data.quantity,
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

                        my_master.title = $scope.master.runtime[0].slave_id+':6060';   //master节点需要用到slave的消息，所以写在这里
                        temp_slave.label = "slave" + cur_index;
                        temp_slave.id = cur_index;
                        temp_slave.shape = 'image';
                        temp_slave.image = DIR + 'Hardware-WQN-server.png';
                        temp_slave.title = $scope.master.runtime[i].slave_id;
                        vertexes[cur_index] = temp_slave; // cur_index 同时代表 顶点集合 my_slaves 的下标

                        var temp_edge = {}; // 添加一条边 master -> temp_slave
                        temp_edge.from = 0;
                        temp_edge.to = temp_slave.id;
                        temp_edge.arrows = 'to';
                        temp_edge.label = Math.round($scope.master.runtime[i].net_usage.net_used * 100) / 100 + 'KiB/s';
                        index_edge++; // 边集合 my_edges 的下标
                        my_edges[index_edge] = temp_edge;


                    }
                } else      {
                    my_edges = [];
                }

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

                    if($scope.master.framework.quantities >=1)
                    {
                        //var my_edges = [];
                        var framework_index = cur_index;
                        for (var f in $scope.master.framework.resource)
                        {
                            //   添加framework运行节点
                            var temp_framework = {};
                            framework_index++;
                            temp_framework.label = $scope.master.framework.resource[f].name;
                            temp_framework.id = framework_index;
                            temp_framework.shape = 'image';
                            temp_framework.image =DIR + 'Hardware-WQN-spark.png'
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
                                color: "#c50ac2" // blue
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


    //开启supermaster对应的Controller
    chameleon_app.controller('StartSupermasterCtrl', function ($scope, $rootScope, $uibModal) {
        //$scope.startSupermaster = response.data.start;
        console.log('8' + $rootScope.startSupermaster);
        console.log('88'+ $rootScope.stopSupermaster);
        $scope.openModal = function () {
            if ($rootScope.stopSupermaster == 'success'||$rootScope.startSupermaster == undefined) {
                //$scope.startSupermaster = $rootScope.startSupermaster;
                var alert_message = "确认开启Supermaster?";
                console.log('9' + $rootScope.startSupermaster);
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
            $scope.openModal = function () {
                alert("supermaster已经开启");

            }
        }
    });

    chameleon_app.controller('StartThreeSupermasterCtrl', function ($scope, $rootScope, $uibModal) {
        $scope.openModal = function () {
            if ($rootScope.stopSupermaster == 'success'||$rootScope.startSupermaster == undefined) {
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
            $scope.openModal = function () {
                alert("supermaster已经开启");

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
        console.log('5' + $rootScope.stopSupermaster);

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
            $scope.openModal = function () {
                alert("supermaster已经关闭");

            }
        }

    });

    chameleon_app.controller('StopSupermasterInstanceCtrl', function($scope,$rootScope, $uibModalInstance,$http, date) {
        $scope.date= date;

        //在这里处理要进行的操作
        $scope.ok = function() {
            $http({
                method: 'GET',
                url: leadingChameleonSuperMasterURL('/super_master/kill_master')
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


})();