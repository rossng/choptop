<template>
	<div class="timer">
		{{name}} {{timeString(currentTime)}} of {{timeString(startTime)}}
	</div>
</template>

<script>
	export default {
	  name: 'timer',
	  props:['eventBus', 'running', 'startTime', 'name'],
	  data () {
	    return {
	      currentTime:0,
	    }
	  },
	  created() {
	  	this.currentTime = this.startTime
	    window.setInterval(this.timerTick,1000);
	  },
	  methods:{
	  	timerTick(){
	  		if(!this.running || this.currentTime <=0) return;

	  		this.currentTime -= 1/60;
	  	},
	  	timeString(minutes){
	  		if(minutes < 0) minutes = 0;
	  		var mins = Math.floor(minutes);
	  		var secs = Math.ceil((minutes % 1) * 60);
	  		var secsString = secs;
	  		if(secs < 10){
	  			secsString = "0" + secsString;
	  		}

	  		return mins + ":" + secsString

	  	}
	  }
	}
</script>



<style scoped>
	.timer{
		position: fixed;
		bottom: 45px;
		text-align: center;
		width: 800px;
		left: 200px;
	}
	
</style>
