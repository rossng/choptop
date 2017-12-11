<template>
	<div class = "tapPosition" :style="getStyle()">
		
	</div>
</template>

<script>
	// import AnimatedInt from './AnimatedInteger'
	export default {
	  name: 'tapPosition',
	  props:['eventBus'],
	  components:{
	  	// AnimatedInt
	  },

	  created (){
	  	this.eventBus.$on("pos", this.handleNewPos);
	  },
	  methods:{
	  	handleNewPos(event){
	  		this.latestPos=event.pos;
	  		this.latestScale=event.scale;
	  		console.log("x - y position: " + this.latestPos[0] +" - "+this.latestPos[1]+" scale: "+this.latestScale);
	  	},
	  	getStyle(){
	  		var text = '';
	  		if (this.latestPos ==null){
	  			return ''; 
	  		}
	  		text += "transform: translate("+this.latestPos[0]*800+"px,"+ this.latestPos[1]*480+"px) scale("+this.latestScale+")";
	  		return text;
	  	}
	  },
	  data () {
	    return {
	    	latestPos: null,
	    	latestScale: 0,
	    }
	  }
	  
	}
</script>



<style scoped>
	.tapPosition{
		position: fixed;
		left: 0px;
		top:0px;
		background: black;
		width: 30px;
		display: block;
		height: 30px;
		margin-top: -15px;
		margin-left: -15px;
		border-radius: 15px;
		transition : 0.5s;
	}

	
</style>
