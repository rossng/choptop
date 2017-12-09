<template>
	<div :class="getClass()">
		<div v-if="!selected">
			<h1>{{recipe.title}}</h1>
			<h2>Difficulty: {{recipe.difficulty}}</h2>
			<h2>Time: {{recipe.time}} minutes</h2>
		</div>
		<div v-if="selected && !stepsVisible">
			<h1>{{recipe.title}}</h1>
			<h2>Difficulty: {{recipe.difficulty}}</h2>
			<h2>Time: {{recipe.time}} minutes</h2>

			<div id="ingredients">
				Ingredients
				<div id="ingredientsList">
					<div v-for="(ingredient, idx) in this.recipe.ingredients" class="ingredient">
						{{getIngredientText(ingredient)}}
					</div>
				</div>
			</div>
		</div>	

		<div v-if="selected && stepsVisible">	
			<div id="steps">
				<StepDisplay :steps="recipe.steps" :stepIdx="selectedStep"/>
			</div>
			<div id="timers">
				<Timer v-for="(timer, idx) in this.activeTimers" :key="idx" :running="timer.running" :name="timer.name" :startTime="timer.startTime"/>
			</div>
		</div>

	</div>
</template>

<script>

	import StepDisplay from './StepDisplay'
	import Timer from './Timer'

	export default {
	  name: 'recipeOverview',
	  props:['recipe', 'hovered', 'selected', "eventBus"],
	  data () {
	    return {
	      selectedStep:0,
	      focussed: false,
	      stepsVisible:false,
	      activeTimers: [],
	    }
	  },
	  created(){
			this.eventBus.$on("pressed", this.handlePress);
	  },

	  components:{
	  	StepDisplay,
	  	Timer
	  },
	  
	  methods: {
	  	handlePress(dir){
			if (this.selected){
				if(dir == "right"){
					console.log("right step")
					this.nextStep();
				}else if(dir =="left"){
					this.prevStep();
				}else if(dir == "down"){
					console.log("steps visible")
					if(!this.stepsVisible){
						this.stepsVisible = true;
					}else{
						this.startTimer();
					}
				}else if(dir == "up"){
					if(this.stepsVisible){
						this.stepsVisible = false;
					}else{
						this.$parent.upPressed();
					}
				}
			}
		},
	  	nextStep: function(){
	  		if(this.selectedStep < this.recipe.steps.length - 1){
	  			this.selectedStep++;
	  		}
	  	},
	  	startTimer(){
	  		if(this.hasTimeAtCurrentStep()){
	  			console.log("starting timer for " + this.getStepTime() + "mins for " + this.getTimerName());
	  			var timerName = this.getTimerName();

	  			if(this.timerSet(timerName)){
	  				console.log("timer already set for " + timerName);
	  				return; //since timer is already set
	  			}

	  			var timer = {
	  				startTime: this.getStepTime(),
	  				name: timerName,
	  				running: true,
	  			}

	  			this.activeTimers.push(timer)
	  		}
	  	},
	  	hasTimeAtCurrentStep(){
		  		return this.recipe.steps[this.selectedStep].time != undefined;
		},

		getStepTime(){
			return this.recipe.steps[this.selectedStep].time;
		},
		getTimerName(){
			var tn = this.recipe.steps[this.selectedStep].timerName;
			if (tn != undefined) return tn;
			return "Timer";
		},
		timerSet(name){
			for (var timer of this.activeTimers){
				if(timer.name == name){
					return true;
				}
			}
			return false;
		},

	  	prevStep: function(){
	  		if(this.selectedStep > 0){
	  			this.selectedStep--;
	  		}
	  	},
	  	getClass : function(){
	  		var divClass = "recipeOverview"
	  		if(this.hovered && !this.selected){
	  			divClass += " hovered"
	  		}

	  		if(this.selected){
	  			divClass += " selected"
	  		}
	  		return divClass
	  	},

	  	getIngredientText(ingredient){
	  		//this code isn't nice, sorry
	  		var text = "";
	  		if(ingredient.units.length == 2){
	  			// 
	  			if(ingredient.units[0] == "each"){
	  				var quantity = ingredient.quantity[0]
	  				text += quantity + " "
	  				text += ingredient.name
	  				if(quantity > 1 && ingredient.name[ingredient.name.length -1] != s){
	  					text += "s" //pluralise names
	  				}
	  				text += " ("+ingredient.quantity[1]+ " " + ingredient.units[1]+") "
	  			}
	  		}else {
	  			if (ingredient.units == "each"){
	  				var quantity = ingredient.quantity[0]
	  				text += quantity + " "
	  				text += ingredient.name
	  			}else if (ingredient.units == "to taste"){
	  				text += ingredient.name + " "
	  				text += ingredient.units
	  				
	  			}
	  			else{
	  				var quantity = ingredient.quantity[0]
	  				text += quantity + " " + ingredient.units+" "
	  				text += ingredient.name
	  			}
	  		}
	  		return text
	  	}
	  }
	}
</script>



<style scoped>
	.recipeOverview{
		border:1px black solid;
		margin:10px;
	}

	.recipeOverview.hovered{
		background:#ffe6bf;
	}

	.recipeOverview.selected{
		background: #68b4b2;
		/*height: 100%;
    	width: 100%;*/
	}

	.ingredient{
		width:50%;
		display:inline-block;
		float:left;
	}

	#ingredientsList{
		clear:both;
		overflow:hidden;
	}
	
</style>
