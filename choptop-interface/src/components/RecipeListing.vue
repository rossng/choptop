<template>
	<div class="recipeOverviewListing">
		<div v-if="!this.selected" :class="{arr:true, leftarr:true}"><icon name="angle-left" scale="4"></icon></div>
		<RecipeOverview v-for="(rec, index)  in recipes" :key="rec.name" :eventBus="eventBus" v-if="!selected || hoveredRecipeIndex ==index"  :recipe="rec" :hovered="index == hoveredRecipeIndex" :selected="index == hoveredRecipeIndex && selected" :portions="portionCount"/>
		<div v-if="!this.selected" :class="{rightArr:true, arr:true}"><icon name="angle-right" scale="4"></icon></div>
	</div>
</template>

<script>
	import RecipeOverview from './RecipeOverview'


	export default {
		name: 'recipeListing',
		components: {
			RecipeOverview
		},

		created(){
			this.eventBus.$on("pressed", this.handlePress);
		},

		methods:{
			handlePress(dir){
			  if (this.focussed == true){
			  	if(dir == "right"){
			  		this.nextRecipe();
			  	}else if(dir =="left"){
			  		this.previousRecipe();
			  	}else if(dir == "down"){
			  		this.selected = true;
			  		this.focussed = false;
			  	}else if(dir == "up"){
			  		this.$parent.upPressed();
			  	}
			  }
			},
			upPressed(){
				this.selected = false;
				this.focussed = true;
			},
			nextRecipe(){
				this.hoveredRecipeIndex = (this.hoveredRecipeIndex + 1) % this.recipes.length
			},
			previousRecipe(){
				this.hoveredRecipeIndex = (this.hoveredRecipeIndex - 1  + this.recipes.length) % this.recipes.length //correct modulo
			}
		},

		props: ['recipes', 'eventBus', 'portionCount'],
		data () {
			return {
				hoveredRecipeIndex: 0,
				selected: false,

				focussed:true
			  
			}
		}
	}
</script>



<style scoped>
	h1{
	
	}

	.recipeOverviewListing{
		display: flex;
  		flex-direction: row;
  		align-items: center;
	  	justify-content: center;
	  	height:100%;
	}


	.arr{
		font-size: 100px;
		opacity: 1;
		transition: 0.5s;
	}
	
</style>
