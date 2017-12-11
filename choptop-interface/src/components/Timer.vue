<template>
	<div :class="getClass()">
		<vue-circle
		ref="circ"
        :progress="100"
        :size="100"
        :reverse="false"
        :fill='{gradient: ["#fdd250", "#3aeabb"], gradientAngle: Math.PI / 4}'
        line-cap="round"
        show-percent="false"
        :animation-start-value="0"
        :start-angle="(3/2)*Math.PI"
        insert-mode="append"
        :thickness="10"
        :show-percent="false">
          <span class="timerValue">{{getTimeString()}}</span>
          <span class="timerName">{{name}}</span>
          <span class="totalTime">{{timeString(startTime)}}</span>
      </vue-circle>
	</div>
</template>

<script>
	import VueCircle from 'vue2-circle-progress';

	export default {
	  name: 'timer',
	  props:['eventBus', 'running', 'startTime', 'name'],
	  components:{
		VueCircle
	  },
	  data () {
	    return {
	      currentTime:0,
	      interval: null
	    }
	  },
	  created() {
	  	this.currentTime = this.startTime
	    this.interval = window.setInterval(this.timerTick,1000);
	  },
	  beforeDestroy(){
	  	clearInterval(this.interval)
	  },
	  methods:{
	  	timerTick(){
	  		if(!this.running || this.currentTime <=0) return;

	  		this.currentTime -= 1/60;
	  		if(this.currentTime <=0){
	  			this.$refs.circ.updateProgress(0.01)	
	  			this.$parent.removeWithDelay(this.name) //remove timer after expiry
	  		}else{
	  			this.$refs.circ.updateProgress((this.currentTime/this.startTime)*100)	
	  		}

	  		this.getClass()
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

	  	},
	  	getClass(){
	  		var str = "timer"
	  		if(this.currentTime <= 0){
	  			str+=" finished"
	  		}
	  		return str;
	  	},
	  	getTimeString(){
	  		return this.timeString(this.currentTime);
	  	}
	  }
	}
</script>



<style>
	.timer{
		display: inline-block;
		text-align: center;
		margin:10px;
	}

	.inner-text{
		color:red;
	}

	.timerValue{
		/*color:red;*/
		margin-top:-12px;
	}
	.timerName{

		/*color:blue;*/
	}
	.timer.finished .timerValue {
  		animation: blinker 1s linear infinite;
  		color:red;
	}

	@keyframes blinker {  
		20% { opacity: 1; }
  		50% { opacity: 0; }
  		90% { opacity: 1; }
	}

	.totalTime{
		font-size:70%;
		margin-top:10px;
	}
	
</style>
