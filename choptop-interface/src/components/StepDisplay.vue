<template>
	<div class="step">
			Step:{{stepIdx+1}}/{{steps.length}}
			{{steps[stepIdx].text}}

	</div>
</template>

<script>


	export default {
		name: 'stepDisplay',
		components: {
		},

		props: ['steps', 'stepIdx'],
		data () {
			return {
				// hoveredRecipeIndex: 0,
				selected: true
			  
			}
		}
	}
</script>



<style scoped>
	h1{
	
	}
	
</style>