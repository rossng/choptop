<template>
	<div class="portionSelector verticalCenter">
		<div>
		<p><h2>How many portions would you like to make?</h2></p>
		<p>
			<h1>{{showPortionCount}} portion<span v-if="showPortionCount > 1">s</span></h1>
		</p>
		</div>
	</div>
</template>

<script>
	export default {
	  name: 'portionSelector',
	  props:['eventBus', 'portionCount'],
	  created (){
	  	this.eventBus.$on("pressed", this.handlePress);
	  	this.showPortionCount = this.portionCount;
	  },
	  data () {
	    return {
	      showPortionCount:0,
	      focussed: true
	    }
	  },
	  methods:{
	  	handlePress(dir){
	  		if(this.focussed){
	  			console.log("portion handle press " + dir)
	  			if (dir == "left" && this.showPortionCount > 1){
	  				this.showPortionCount--;
	  				this.eventBus.$emit("portionCount", this.showPortionCount)
	  			}else if(dir == "right"){
	  				this.showPortionCount++;
	  				this.eventBus.$emit("portionCount", this.showPortionCount)
	  			}else if(dir == "up"){
	  				this.focussed = false;
	  				this.$parent.upPressed();
	  			}else if(dir == "down"){
	  				this.focussed = false;
	  				this.$parent.downPressed();
	  			}
	  		}
	  	}
	  }
	}
</script>



<style scoped>

	
</style>
