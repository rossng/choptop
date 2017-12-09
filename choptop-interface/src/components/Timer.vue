<template>
	<div class="timer">
		<vue-circle
		ref="circ"
        :progress="100"
        :size="100"
        :reverse="false"
        line-cap="round"
        inner-text="getTimeString()"
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
	  		if(this.currentTime <=0){
	  			this.$refs.circ.updateProgress(0.01)	
	  		}else{
	  			this.$refs.circ.updateProgress((this.currentTime/this.startTime)*100)	
	  		}
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
	  	getTimeString(){
	  		return this.timeString(this.currentTime);
	  	}
	  }
	}
</script>



<style>
	.timer{
		bottom: 45px;
		text-align: center;
		width: 800px;
		left: 200px;
	}

	.inner-text{
		color:red;
	}

	.timerValue{
		color:red;
		margin-top:-12px;
	}
	.timerName{

		color:blue;
	}

	.totalTime{
		font-size:70%;
		margin-top:10px;
	}
	
</style>
